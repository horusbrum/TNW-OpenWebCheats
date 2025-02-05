/*
*   Copyright (C) {2015}  {Victor Klafke, Charles TheHouse}
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see [http://www.gnu.org/licenses/].
*
*   Contact at: victor.klafke@ecomp.ufsm.br
*/
#include "ProcessClientMessage.h"

void Exec_MSG_Deposit(int conn, char *pMsg)
{
	MSG_STANDARDPARM *m = (MSG_STANDARDPARM*)pMsg;

	int Size = m->Size;

	if (Size > sizeof(MSG_STANDARDPARM)) //CONTROLE DE SIZE
	{
		SendClientMessage(conn, "Imposs�vel executar a��o25, tente mais tarde. ");
		return;
	}
	if (pUser[conn].Atraso != 0)
	{
		int isTime = GetTickCount64() - pUser[conn].Atraso; // ATRASO PACOTE

		if (isTime < 800)
		{
			SendClientMessage(conn, "Aguarde 1 segundo para uma nova Tentativa.");
			return;
		}

	}
	pUser[conn].Atraso = GetTickCount64();

	if (pMob[conn].MOB.CurrentScore.Hp == 0 || pUser[conn].Mode != USER_PLAY)
	{
		SendHpMode(conn);
		AddCrackError(conn, 10, 1);
		return;
	}

	int coin = m->Parm;

	if (coin < 0 || coin > 2000000000)
		return;

	if (pMob[conn].MOB.Coin >= coin && coin >= 0 && coin <= 2000000000)
	{
		int tcoin = coin + pUser[conn].Coin;

		if (tcoin >= 0 && tcoin <= 2000000000)
		{
			pMob[conn].MOB.Coin -= coin;
			pUser[conn].Coin = tcoin;

			m->ID = ESCENE_FIELD;

			int Size = m->Size;

			if (Size > sizeof(MSG_STANDARDPARM))
			{
				m->Size = 0;
				return;
			}

			if (!pUser[conn].cSock.AddMessage((char*)m, sizeof(MSG_STANDARDPARM)))
				CloseUser(conn);

			SendCargoCoin(conn);

			//sprintf(temp, "etc,deposito no bau D:%d R:%d", coin, tcoin);
			//Log(temp, pUser[conn].AccountName, pUser[conn].IP);
		}
		else
			SendClientMessage(conn, g_pMessageStringTable[_NN_Cant_get_more_than_2G]);
	}
	else
		SendClientMessage(conn, g_pMessageStringTable[_NN_Cant_Deposit_That_Much]);
}