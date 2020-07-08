// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}




// Battleground Extended by Vykimo
ACMD_FUNC(bgstart) {
	
	nullpo_retr(-1, sd);
	
	if (bg_flag) {
		clif_displaymessage(fd, "Battleground is currently in progress.");
		return -1;
	}

	bg_flag = true;
	bg_start();
	clif_displaymessage(fd, "Battleground has been initiated");

	return 0;
}

ACMD_FUNC(bgend) {
	
	nullpo_retr(-1, sd);
	
	if (!bg_flag) {
		clif_displaymessage(fd, "Battleground is currently not in progress.");
		return -1;
	}

	bg_flag = false;
	bg_end();
	clif_displaymessage(fd, "Battleground has been ended.");

	return 0;
}

ACMD_FUNC(listenbg)
{
	sd->state.bg_listen = (sd->state.bg_listen == 0);
	
	if( sd->state.bg_listen )
		clif_displaymessage(fd, "You will receive Battleground announcements.");
	else
		clif_displaymessage(fd, "You will not receive Battleground announcements.");

	return 0;
}

ACMD_FUNC(order)
{
	nullpo_retr(-1,sd);
	if( !message || !*message )
	{
		clif_displaymessage(fd, "Please, enter a message (usage: @order <message>).");
		return -1;
	}

	if( map_getmapflag(sd->bl.m, MF_BATTLEGROUND) )
	{
		if( !sd->bmaster_flag )
		{
			clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
			return -1;
		}
		clif_broadcast2(&sd->bl, message, (int)strlen(message)+1, sd->bmaster_flag->color, 0x190, 20, 0, 0, BG);
	}
	else
	{
		if( !sd->state.gmaster_flag )
		{
			clif_displaymessage(fd, "This command is reserved for Guild Leaders Only.");
			return -1;
		}
		clif_broadcast2(&sd->bl, message, (int)strlen(message)+1, 0xFF0000, 0x190, 20, 0, 0, GUILD);
	}

	return 0;
}
ACMD_FUNC(leader)
{
	struct map_session_data *pl_sd;
	nullpo_retr(-1,sd);
	if( !sd->bmaster_flag )
		clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
	else if( sd->ud.skilltimer != INVALID_TIMER )
		clif_displaymessage(fd, "Command not allow while casting a skill.");
	else if( !message || !*message )
		clif_displaymessage(fd, "Please, enter the new Leader name (usage: @leader <name>).");
	else if( (pl_sd = map_nick2sd((char *)message,false)) == NULL )
		clif_displaymessage(fd, msg_txt(sd,3)); // Character not found.
	else if( sd->bg_id != pl_sd->bg_id )
		clif_displaymessage(fd, "Destination Player is not in your Team.");
	else if( sd == pl_sd )
		clif_displaymessage(fd, "You are already the Team Leader.");
	else
	{ // Everytest OK!
		sprintf(atcmd_output, "Team Leader transfered to [%s]", pl_sd->status.name);
		clif_broadcast2(&sd->bl, atcmd_output, (int)strlen(atcmd_output)+1, sd->bmaster_flag->color, 0x190, 20, 0, 0, BG);

		sd->bmaster_flag->leader_char_id = pl_sd->status.char_id;
		pl_sd->bmaster_flag = sd->bmaster_flag;
		sd->bmaster_flag = NULL;

		clif_name_area(&sd->bl);
		clif_name_area(&pl_sd->bl);
		return 0;
	}
	return -1;
}

ACMD_FUNC(reportafk)
{
	struct map_session_data *pl_sd;
	nullpo_retr(-1,sd);
	if( !sd->bg_id )
		clif_displaymessage(fd, "This command is reserved for Battleground Only.");
	else if( !sd->bmaster_flag && battle_config.bg_reportafk_leaderonly )
		clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
	else if( !message || !*message )
		clif_displaymessage(fd, "Please, enter the character name (usage: @reportafk/votekick <name>).");
	else if( (pl_sd = map_nick2sd((char *)message,false)) == NULL )
		clif_displaymessage(fd, msg_txt(sd,3)); // Character not found.
	else if( sd->bg_id != pl_sd->bg_id )
		clif_displaymessage(fd, "Destination Player is not in your Team.");
	else if( sd == pl_sd )
		clif_displaymessage(fd, "You cannot kick yourself.");
	else if( pl_sd->state.bg_afk == 0 )
		clif_displaymessage(fd, "The player is not AFK on this Battleground.");
	else
	{ // Everytest OK!
		struct battleground_data *bg;
		if( (bg = bg_team_search(sd->bg_id)) == NULL )
			return -1;

		bg_team_leave(pl_sd,2);
		clif_displaymessage(pl_sd->fd, "You have been kicked from Battleground because of your AFK status.");
		pc_setpos(pl_sd,pl_sd->status.save_point.map,pl_sd->status.save_point.x,pl_sd->status.save_point.y,CLR_OUTSIGHT);
		clif_refresh(pl_sd);

		sprintf(atcmd_output, "- AFK [%s] Kicked -", pl_sd->status.name);
		clif_broadcast2(&sd->bl, atcmd_output, (int)strlen(atcmd_output)+1, bg->color, 0x190, 20, 0, 0, BG);
		return 0;
	}
	return -1;
}
/*==========================================
 * Guild Skill Usage for Guild Masters
 *------------------------------------------*/
ACMD_FUNC(guildskill)
{
	int i, skillnum = 0, skilllv = 0;
	t_tick tick = gettick();

	const struct { char skillstr[3]; int id; } skills[] = {
		{ "BO",	10010 },
		{ "RG",	10011 },
		{ "RS",	10012 },
		{ "EC",	10013 },
	};

	// Check for Skill ID
	for( i = 0; i < ARRAYLENGTH(skills); i++ )
	{
		if( strncmpi(message, skills[i].skillstr, 3) == 0 )
		{
			skillnum = skills[i].id;
			break;
		}
	}
	if( !skillnum )
	{
		clif_displaymessage(fd, "Invalid Skill string. Use @guildskill EC/RS/RG/BO");
		return -1;
	}

	if( !map_getmapflag(sd->bl.m, MF_BATTLEGROUND) ) {
		if( sd->state.gmaster_flag )
			skilllv = guild_checkskill(sd->guild, skillnum);
		else {
			clif_displaymessage(fd, "This command is reserved for Guild Leaders Only.");
			return -1;
		}
	} else {
		struct battleground_data *bg;
		if( (bg = sd->bmaster_flag) != NULL )
			skilllv = bg_checkskill(bg, skillnum);
		else {
			clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
			return -1;
		}
	}

	if( pc_cant_act(sd) || pc_issit(sd) || skill_isNotOk(skillnum, sd) || sd->ud.skilltimer != -1 || sd->sc.option&(OPTION_WEDDING|OPTION_XMAS|OPTION_SUMMER) || sd->state.only_walk || sd->sc.data[SC_BASILICA] )
		return -1;

	if( DIFF_TICK(tick, sd->ud.canact_tick) < 0 )
		return -1;

	if( sd->menuskill_id )
	{
		if( sd->menuskill_id == SA_TAMINGMONSTER )
			sd->menuskill_id = sd->menuskill_val = 0; //Cancel pet capture.
		else if( sd->menuskill_id != SA_AUTOSPELL )
			return -1; //Can't use skills while a menu is open.
	}

	sd->skillitem = sd->skillitemlv = 0;
	if( skilllv ) unit_skilluse_id(&sd->bl, sd->bl.id, skillnum, skilllv);
	return 0;
}
