#include "LogExpression.h"
#include "Log.h"
#include <include/CharacterLib/Character.h>

#define _SELF L"CLogExpression.cpp"
namespace libTools
{
	CExprFunBase::CExprFunBase()
	{

	}

	UINT CLogExpression::Push(_In_ std::function<VOID(CONST std::vector<std::wstring>&)> fnPtr, _In_ CONST std::wstring& wsFunName) throw()
	{
		ExpressionFunPtr FunPtrCustome_ = { fnPtr, wsFunName };
		VecFunPtr.push_back(FunPtrCustome_);
		return static_cast<UINT>(VecFunPtr.size());
	}

	VOID CLogExpression::SetVecExprFunPtr(_In_ CONST std::vector<ExpressionFunPtr>& Vec ) throw()
	{
		VecFunPtr = Vec;
	}

	BOOL CLogExpression::Run(_In_ CONST std::wstring& wsText) throw()
	{
		if (wsText.find(L"(") == -1 || wsText.find(L")") == -1)
		{
			LOG_CF_E(L"'%s' UnExist '(' or ')'", wsText.c_str());
			return FALSE;
		}
		if (CCharacter::GetCount_By_SpecifyText<WCHAR>(wsText, L"\"") % 2 != 0)
		{
			LOG_CF_E(L"'\"' have to make a pair!");
			return FALSE;
		}

		std::wstring wsFunName;
		std::wstring wsExpText = wsText;
		CCharacter::GetRemoveLeft<WCHAR>(wsExpText, L"(", wsFunName);
		CCharacter::GetRemoveRight<WCHAR>(wsExpText, L"(", wsExpText);
		CCharacter::GetRemoveLeft<WCHAR>(wsExpText, L")", wsExpText);

		std::vector<std::wstring> VecParm;
		CCharacter::GetVecByParm_RemoveQuotes(wsExpText, ',', VecParm);

		auto itr = std::find_if(VecFunPtr.begin(),VecFunPtr.end(), [wsFunName](ExpressionFunPtr& ExprFunPtr) { return ExprFunPtr.wsFunName == wsFunName; });
		if (itr != VecFunPtr.end())
		{
			itr->fnPtr(VecParm);
			return TRUE;
		}

		LOG_CF_E(L"UnExist Function Name '%s'", wsFunName.c_str());
		itr = std::find_if(VecFunPtr.begin(), VecFunPtr.end(), [](ExpressionFunPtr& ExprFunPtr) { return ExprFunPtr.wsFunName == L"Help"; });
		if (itr != VecFunPtr.end())
		{
			itr->fnPtr(VecParm);
		}
		return FALSE;
	}
}
