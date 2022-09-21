#include "stdafx.h"
#include "..\Public\Trail_Bow.h"
#include "GameInstance.h"

CTrail_Bow::CTrail_Bow(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CTrail_Bow::CTrail_Bow(const CTrail_Bow & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrail_Bow::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Bow::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	WRAP temp;

	memcpy(&temp, pArg, sizeof(WRAP));

	m_Target = temp.pTarget;

	if (m_Target == nullptr)
		return E_FAIL;
	
//	m_pVIBufferCom->Set_TrailPoint(XMVectorSet(-0.1f, 0.f, -0.5f, 0.f), XMVectorSet(0.1f, 0.f, -0.5f, 0.f));

	m_pVIBufferCom2->Set_TrailPoint(XMVectorSet(0.f, 0.1f,0.f, 0.f), XMVectorSet(0.f, -0.1f, 0.f, 0.f));

	//m_pVIBufferCom->Set_MaxFrame(2);

	m_pVIBufferCom2->Set_MaxFrame(2);

	//버퍼렉트2 릴리즈

	return S_OK;
}

_int CTrail_Bow::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_Target->Get_isDead() == true) {
		m_Dead = true;
		return 1;
	}
	
	//m_pTransformCom->MyLookSet(m_Target->Get_Transform()->Get_State(CTransform::STATE_LOOK));


	m_pVIBufferCom->Update(TimeDelta,m_Target->Get_Transform()->Get_WorldMatrix());
	m_pVIBufferCom2->Update(TimeDelta, m_Target->Get_Transform()->Get_WorldMatrix());
	return 0;
}

void CTrail_Bow::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	Get_CamearaDistance();
	//ZBillBoard();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CTrail_Bow::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;
	
	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;

	if (FAILED(m_pVIBufferCom2->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Bow::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Bow::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("MyColor", &m_Color, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTrail_Bow * CTrail_Bow::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CTrail_Bow*	pInstance = new CTrail_Bow(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CTrail_Bow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrail_Bow::Clone(void * pArg)
{
	CTrail_Bow*	pInstance = new CTrail_Bow(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CTrail_Bow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Bow::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);

}
