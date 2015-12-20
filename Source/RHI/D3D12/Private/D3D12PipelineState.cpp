#include "DXCommon.h"
#include "Public/D3D12RHI.h"
#include "D3D12RootSignature.h"
#include "D3D12Enums.h"

NS_K3D_D3D12_BEGIN

PipelineLayout::PipelineLayout()
{

}

PipelineLayout::PipelineLayout(rhi::ShaderParamLayout const &ShaderParamLayout)
{
	CreateFromShaderLayout(ShaderParamLayout);
}

PipelineLayout::~PipelineLayout()
{

}

void PipelineLayout::Create(rhi::ShaderParamLayout const & ShaderParamLayout)
{
	CreateFromShaderLayout(ShaderParamLayout);
}

void PipelineLayout::Finalize(rhi::IDevice * pRHIDevice)
{
	Device* pDevice = static_cast<Device*>(pRHIDevice);
	K3D_ASSERT(pDevice != nullptr);
	m_RootSignature.Finalize(pDevice->Get(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
}

void PipelineLayout::CreateFromShaderLayout(rhi::ShaderParamLayout const &)
{

}

PipelineState::PipelineState(Device::Ptr pDevice)
	: D3D12RHIDeviceChild(pDevice)
	, m_GraphicsPSODesc{}
	, m_ComputePSODesc{}
{
}

PipelineState::~PipelineState()
{}

void PipelineState::SetShader(rhi::EShaderType Type, rhi::IShaderBytes* RShader)
{
	ShaderBytes * Sb = static_cast<ShaderBytes*>(RShader);
	K3D_ASSERT(Sb != nullptr);
	switch (Type)
	{
	case rhi::ES_Vertex:
		m_GraphicsPSODesc.VS = Sb->AsBC();
		break;
	case rhi::ES_Fragment:
		m_GraphicsPSODesc.PS = Sb->AsBC();
		break;
	case rhi::ES_Compute:
		m_ComputePSODesc.CS = Sb->AsBC();
		break;
	case rhi::ES_Hull:
		m_GraphicsPSODesc.HS = Sb->AsBC();
		break;
	case rhi::ES_Domain:
		m_GraphicsPSODesc.DS = Sb->AsBC();
		break;
	case rhi::ES_Geometry:
		m_GraphicsPSODesc.GS = Sb->AsBC();
		break;
	}
}

void PipelineState::DestroyAll()
{

}

void PipelineState::SetLayout(rhi::IPipelineLayout *Layout)
{
	PipelineLayout* pLayout = static_cast<PipelineLayout*>(Layout);
	SetRootSignature(pLayout->m_RootSignature);
}

void PipelineState::SetRootSignature(const RootSignature& RootSig)
{
	if (GetType() == rhi::EPipelineType::EPSO_Graphics)
		m_GraphicsPSODesc.pRootSignature = RootSig.GetSignature();
	else
		m_ComputePSODesc.pRootSignature = RootSig.GetSignature();
}

void PipelineState::Finalize()
{
	if (GetType() == rhi::EPipelineType::EPSO_Compute)
	{
		ThrowIfFailed(GetParentDeviceRef().Get()->CreateComputePipelineState(&m_ComputePSODesc, IID_PPV_ARGS(m_pPSO.GetAddressOf())));
	}
	else
	{
		/*
		m_GraphicsPSODesc.SampleMask = UINT_MAX;
		m_GraphicsPSODesc.NumRenderTargets = 1;
		m_GraphicsPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_GraphicsPSODesc.SampleDesc.Count = 1;
		*/
		ThrowIfFailed(GetParentDeviceRef().Get()->CreateGraphicsPipelineState(&m_GraphicsPSODesc, IID_PPV_ARGS(m_pPSO.GetAddressOf())));
	}
}

const RootSignature & PipelineState::GetRootSignature() const
{
	K3D_ASSERT(m_RootSignature != nullptr, "RootSignature is null");
	return *m_RootSignature;
}

GraphicsPSO::GraphicsPSO(Device::Ptr pDevice)
	: PipelineState(pDevice)
{
}

GraphicsPSO::~GraphicsPSO()
{
}

void GraphicsPSO::SetRasterizerState(const rhi::RasterizerState& RState)
{
	RHIRasterizerDesc(m_GraphicsPSODesc.RasterizerState, RState);
}

void GraphicsPSO::SetBlendState(const rhi::BlendState& BState)
{
	RHIBlendDesc(m_GraphicsPSODesc.BlendState, BState);
}

void GraphicsPSO::SetDepthStencilState(const rhi::DepthStencilState& DState)
{
	RHIDepthStencilDesc(m_GraphicsPSODesc.DepthStencilState, DState);
}

void GraphicsPSO::SetSampler(rhi::ISampler* RHISampler)
{
	Sampler * sampler = reinterpret_cast<Sampler*>(RHISampler);
	
}

void GraphicsPSO::SetVertexInputLayout(rhi::VertexDeclaration *, uint32 Count)
{
	/*
	VertexInputLayout * layout = static_cast<VertexInputLayout*>(RHILayout);
	m_GraphicsPSODesc.InputLayout = {layout->m_InputLayout.get(), layout->m_ElementCount};*/
}

void GraphicsPSO::SetPrimitiveTopology(const rhi::EPrimitiveType TopologyType)
{
	switch(TopologyType) 
	{
	case rhi::EPT_Triangles:
	case rhi::EPT_TriangleStrip:
		m_GraphicsPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	case rhi::EPT_Points:
		m_GraphicsPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	case rhi::EPT_Lines:
		m_GraphicsPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	default:
		m_GraphicsPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		break;
	}
}

void GraphicsPSO::SetRenderTargetFormat(const rhi::RenderTargetFormat & RTFormat)
{
	DXGI_FORMAT * RTFormats = (DXGI_FORMAT *)calloc(RTFormat.NumRTs, sizeof(DXGI_FORMAT));
	for (uint32 i = 0; i < RTFormat.NumRTs; i++)
	{
		RTFormats[i] = g_DXGIFormatTable[RTFormat.RenderPixelFormats[i]];
	}
	SetRenderTargetFormats(RTFormat.NumRTs, RTFormats, g_DXGIFormatTable[RTFormat.DepthPixelFormat], RTFormat.MSAACount, 0);
}

void GraphicsPSO::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT * RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	K3D_ASSERT(NumRTVs == 0 || RTVFormats != nullptr, "Null format array conflicts with non-zero length");
	for (UINT i = 0; i < NumRTVs; ++i)
		m_GraphicsPSODesc.RTVFormats[i] = RTVFormats[i];
	for (UINT i = NumRTVs; i < m_GraphicsPSODesc.NumRenderTargets; ++i)
		m_GraphicsPSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	m_GraphicsPSODesc.NumRenderTargets = NumRTVs;
	m_GraphicsPSODesc.DSVFormat = DSVFormat;
	m_GraphicsPSODesc.SampleDesc.Count = MsaaCount;
	m_GraphicsPSODesc.SampleDesc.Quality = MsaaQuality;
}

ComputePSO::ComputePSO(Device::Ptr pDevice)
	: PipelineState(pDevice)
{}

ComputePSO::~ComputePSO()
{}

NS_K3D_D3D12_END

namespace rhi
{
	IPipelineLayout* CreatePipelineLayout(ShaderParamLayout const &Layout)
	{
		return new ::k3d::d3d12::PipelineLayout(Layout);
	}
}