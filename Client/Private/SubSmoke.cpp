#include "stdafx.h"
#include "..\Public\SubSmoke.h"
#include "GameInstance.h"

CSubSmoke::CSubSmoke(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CSubSmoke::CSubSmoke(const CSubSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSubSmoke::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSubSmoke::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;
	
	_vector vPos;
	memcpy(&vPos, pArg, sizeof(_vector));
	
	vPos = XMVectorSetW(vPos, 1.f);
	
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.f);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Scaled(_float3(3.f, 3.f, 1.f));

	LookAtCameara();
	return S_OK;
}

_int CSubSmoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	if (m_Frame < m_MaxFrame)
		m_Frame++;
	else {
		m_Frame = 0;
		m_ImgIndex += 1;
		if (m_ImgIndex >= m_ImgCnt)
			return 1;
	}

	return 0;
}

void CSubSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	LookAtCameara();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CSubSmoke::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(11)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSubSmoke::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SubSmoke"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSubSmoke::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", m_ImgIndex)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSubSmoke * CSubSmoke::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CSubSmoke*	pInstance = new CSubSmoke(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CSubSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubSmoke::Clone(void * pArg)
{
	CSubSmoke*	pInstance = new CSubSmoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CSubSmoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubSmoke::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
