// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULingoGameInstanceSubsystem.h"

/* -----------------------------------------------------------
 * UserInfo 관리
 * ----------------------------------------------------------- */

void ULingoGameInstanceSubsystem::SetUserInfo(const FResponseUserMe& InInfo)
{
	UserInfo = InInfo;
}