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

void Exec_MSG_Motion(int conn, char *pMsg)
{
	MSG_Motion *m = (MSG_Motion*)pMsg;

    int Size = m->Size;

    if (Size > sizeof(MSG_Motion)) //CONTROLE DE SIZE
    {
        SendClientMessage(conn, "Imposs�vel executar a��o32, tente mais tarde. ");
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

		AddCrackError(conn, 4, 6);

		return;
	}

	sprintf(temp, "etc,motion motion:%d parm:%d notused:%d", m->Motion, m->Parm, m->NotUsed);
    SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, temp);

	GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_STANDARD*)m, 0);

    // valida��o de User
    if (m->ID != conn)
        return;

    // valida��o de Motion
    if (m->Motion < 0 || m->Motion > 27)
    {
        m->Motion = 0;
        return;
    }
    // 14 0 6F 4D 6A 3 91 0 D5 18 61 0 E C1 C4 C1 0 0 0 0   --> pacote enviado pelo wpe crashando client de users pr�ximos
    // etc,motion motion:6926 parm:1267 notused:0

    // valaida��o do Parm
    if (m->Parm < 0 || m->Parm > 27)
    {
        m->Parm = 0;
        return;
    }
}