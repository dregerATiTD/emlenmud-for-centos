#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef netBSD
#include <unistd.h>
#endif
#include "emlen.h"
#include "mush.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif
#define READ_YESNO(var) strcpy(buf,fread_word(fp)); if (UPPER(buf[0])=='Y') (var)=TRUE; else (var)=FALSE; continue

void 
grab_hostnames (CHAR_DATA * ch, char *argy)
{
#ifndef WINDOWS
	struct sockaddr_in sock;
  struct hostent *from;
  int size;
  char buf[500];
  int b, e;
  DESCRIPTOR_DATA *d;
  DEFINE_COMMAND ("z_grab_hostnames", grab_hostnames, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Grabs hostnames give a specified fd range.")
    argy = one_argy (argy, buf);
  if (buf[0] == '\0' || argy[0] == '\0')
    return;
  if (!is_number (buf) || !is_number (argy))
    return;
  b = atoi (buf);
  e = atoi (argy);
  size = sizeof (sock);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->descriptor > e || d->descriptor < b)
	continue;
      if (getpeername (d->descriptor, (struct sockaddr *) &sock, &size) < 0)
	{
	  continue;
	}
      else
	{
	  int addr;
	  addr = ntohl (sock.sin_addr.s_addr);
	  from = NULL;
	  from = gethostbyaddr ((char *) &sock.sin_addr,
				sizeof (sock.sin_addr), AF_INET);
	  free_string (d->hostname);
	  d->hostname = str_dup (from ? from->h_name : "unknown");
	}
    }
  send_to_char ("Grabbed hostnames in that descriptor range.\n\r", ch);
#endif
  return;
}


COMMAND *
clookup (char *t)
{
  COMMAND *c;
  int i;
  for (i = 0; i < 256; i++)
    {
      for (c = command_hash[i]; c != NULL; c = c->next)
	{
	  if (!str_prefix (t, c->name))
	    return c;
	}
    }
  return NULL;
}

void 
read_command_levels (void)
{
  COMMAND *c;
  FILE *fp;
  char b[200];

  if ((fp = fopen ("commands.dat", "r")) == NULL)
    return;
  for (;;)
    {
      strcpy (b, fread_word (fp));
      if (!str_cmp (b, "END"))
	break;
      if (!str_cmp (b, "LINK"))
	{
	  char t[200];
	  strcpy (b, fread_word (fp));
	  if ((c = clookup (b)) == NULL)
	    {
	      fread_to_eol (fp);
	      continue;
	    }
	  strcpy (t, fread_word (fp));
	  add_to_list (t, c->do_fun, c->position, c->level, c->log, c->help);
	  fprintf (stderr, "Created dynamic command link from '%s' to '%s'.\n", b, t);
	  continue;
	}
      if ((c = clookup (b)) == NULL)
	{
	  fread_number (fp);
	  continue;
	}
      c->level = fread_number (fp);
    }

  fclose (fp);
  return;
}


void 
add_level_bonuses (CHAR_DATA * ch, int how_many)
{
  int i, jh, jm, j, jhr, jmr;
  int hitgain = 0;
  int movegain = 0;
  char buffer[500];
  for (i = 0; i < how_many; i++)
    {
      hitgain = 0;
      movegain = 0;
      if (IS_MOB(ch))
	{
	  int adder;
	  adder = LEVEL(ch)/17;
          if (adder > 8) adder = 8;
	  hitgain = number_range((1 + adder),(4+ adder));
	  movegain = number_range((1 + adder/2),(2 + adder/2));
	}
      else
	{
     
	  jh = 10; 
	  jm = 10;
	  jhr = 10;
	  jmr = 10;
	  
	  for (j = 0; j < 10; j++)
	    {
	      if  ((LEVEL (ch) - i) >= poww.hitgain[j][0] && (LEVEL (ch) - i) <= poww.hitgain[j][1])
		jh = j;
	      if  ((LEVEL (ch) - i) >= poww.movegain[j][0] && (LEVEL (ch) - i) <= poww.movegain[j][1])        
		jm = j;
	      if  ((LEVEL (ch) - i) >= poww.rem_hitgain[j][0] && (LEVEL (ch) - i) <= poww.rem_hitgain[j][1])
		jhr = j;
	      if  ((LEVEL (ch) - i) >= poww.rem_movegain[j][0] && (LEVEL (ch) - i) <= poww.rem_movegain[j][1])
		jmr = j;
	    }
	  
	  { 
	    int remort_mult = 0; /* amt of times remort bonus is added */
	    int up_hitgain = 0;  /* Upper range of the randomness... */
	    int low_hitgain = 0; /* Lower range of randomness */
	    int up_movegain = 0; /* Upper range of randomness for moves */
	    int low_movegain = 0; /* Lower range of the randomness...*/
	    
	    if (poww.remort_bonus_fixed)
	      {
		if (ch->pcdata->remort_times == 0)
		  remort_mult = 0;
		else
		  remort_mult = 1;
	      }
	    else
	      remort_mult = ch->pcdata->remort_times;
	    /* The basic structure of the hitgain/movegain is as follows : */
	    /* From the above we have a list of 4 numbers jh, jhr, jm, jmr which tell 
	       the "place" for hitgain, movegain, remorts hitgain, remorts movegain
	       and then hitgain is made to be the conbonus * (the reg conbonus + the
	       remort bonus times the multiplier). The same idea is used for movegain
	       except now the bonuses are the only things we add and the lower number
	       is the regular number + remortnumber * remort mult. */
	    
	    
	    up_hitgain = poww.hitgain[jh][4] + (ch->pcdata->remort_times) * poww.rem_hitgain[jhr][4];
	    low_hitgain = poww.hitgain[jh][3] + (ch->pcdata->remort_times) * poww.rem_hitgain[jhr][3];
	    hitgain = (con_app[get_curr_con (ch) - 1].hit * ( poww.hitgain[jh][2] + (poww.rem_hitgain[jhr][2] * remort_mult)));
	    hitgain += number_range (low_hitgain, up_hitgain);
	    hitgain /= 100;
	    hitgain += race_info[ch->pcdata->race].hps_bonus;
	    hitgain += align_info[ch->pcdata->alignment].hps_bonus;
	    
	    /* Move gain down here for players... */
	    
	    
	    low_movegain = poww.movegain[jm][2] + (poww.rem_movegain[jmr][2] * remort_mult);
	    up_movegain = poww.movegain[jm][3] + (poww.rem_movegain[jmr][3]  * remort_mult); 
	    movegain = number_range(low_movegain, up_movegain);
	    movegain /= 100;
	    
	  } 
	}
      ch->max_hit += hitgain;
      ch->max_move += movegain;

      sprintf(buffer,"\x1B[1;30m-->\x1B[1;32m%3d hit points\x1B[1;37m and\x1B[1;32m%3d move points\x1B[1;37m gained\n\r",hitgain,movegain);
      send_to_char(buffer,ch);

    }
      MAXMOVE(ch);
      MAXHIT(ch);
      return;
}  

void 
set_initial_hp (CHAR_DATA * ch)
{
  int init_hp;
  int init_mp;
  if (IS_MOB (ch))
    {
      init_hp = poww.initial_hp[0] / 2;	/* Take into account mobs are usually */
      init_mp = (poww.initial_mv[0] * 2) / 3;	/* Higher level */
    }
  else
    {
      if (ch->pcdata->remort_times > 0)
	{
	  init_hp = number_range (poww.remort_hp[0], poww.remort_hp[1]);
	  init_mp = number_range (poww.remort_mv[0], poww.remort_mv[1]);
	}
      else
	{
	  init_hp = number_range (poww.initial_hp[0], poww.initial_hp[1]);
	  init_mp = number_range (poww.initial_mv[0], poww.initial_mv[1]);
	}
    }

  ch->max_hit = init_hp;
  ch->max_move = init_mp;
  if (IS_MOB (ch) || poww.restore_when_level)
    {
      MAXHIT(ch);
      MAXMOVE(ch);
    }
  return;
}

void 
read_power_dat (void)
{
  FILE *fp;
  char buf[80];
  int i, p, y;
  int hg = 0;
  int rhg = 0;
  int mg = 0;
  int rmg = 0;
  int ascremort = 0;
  int remort = 0;
  int guild = 0;
  int augment = 0;
  poww.loot_level=0;
  poww.flee_chance = 0;
  poww.gods=str_dup("<Empty>");
  poww.bash_slip_to_tackle = TRUE;
  poww.penalize_evil_in_sun = TRUE;
  poww.get_from_corpse_while_in_combat = TRUE;
  poww.plague_chance = 0;
  poww.auction_lag=FALSE;
  poww.can_fight_self=TRUE;
  poww.max_level=90;
  poww.good_kill_good=TRUE;
  poww.evil_kill_evil=TRUE;
  poww.all_newbies_good = FALSE;
  poww.level_to_choose = 0;
  poww.choose_in_room = 0;
  

  for (i = 0; i < 20; i++)
    poww.learned[i] = NULL;
  for (p = 0; p < 20; p++)
    poww.armor_lev_word[p] = NULL;
  i = 0;
  p = 0;
  y = 0;
  if ((fp = fopen ("power.dat", "r")) == NULL)
    {
      fprintf (stderr, "Error reading power.dat!\n\r");
      exit (6);
    }
  for (;;)
    {
      strcpy (buf, fread_word (fp));
      if (!str_prefix ("#", buf))
	{
	  fread_to_eol (fp);
	  continue;
	}
      if (!str_cmp ("END", buf))
	break;
     
      if (!str_cmp ("INITIAL_HP", buf))
	{
	  poww.initial_hp[0] = fread_number (fp);
	  poww.initial_hp[1] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REMORT_HP", buf))
	{
	  poww.remort_hp[0] = fread_number (fp);
	  poww.remort_hp[1] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("INITIAL_MV", buf))
	{
	  poww.initial_mv[0] = fread_number (fp);
	  poww.initial_mv[1] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REMORT_MV", buf))
	{
	  poww.remort_mv[0] = fread_number (fp);
	  poww.remort_mv[1] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REG_HITGAIN", buf))
	{
	  poww.hitgain[hg][0] = fread_number (fp);
	  poww.hitgain[hg][1] = fread_number (fp);
	  poww.hitgain[hg][2] = fread_number (fp);
	  poww.hitgain[hg][3] = fread_number (fp);
	  poww.hitgain[hg][4] = fread_number (fp);
	  hg++;
	  continue;
	}
      if (!str_cmp ("REM_HITGAIN", buf))
	{
	  poww.rem_hitgain[rhg][0] = fread_number (fp);
	  poww.rem_hitgain[rhg][1] = fread_number (fp);
	  poww.rem_hitgain[rhg][2] = fread_number (fp);
	  poww.rem_hitgain[rhg][3] = fread_number (fp);
	  poww.rem_hitgain[rhg][4] = fread_number (fp);
	  rhg++;
	  continue;
	}
      if (!str_cmp ("REM_MOVEGAIN", buf))
	{
	  poww.rem_movegain[rmg][0] = fread_number (fp);
	  poww.rem_movegain[rmg][1] = fread_number (fp);
	  poww.rem_movegain[rmg][2] = fread_number (fp);
	  poww.rem_movegain[rmg][3] = fread_number (fp);
	  rmg++;
	  continue;
	}
      if (!str_cmp ("REG_MOVEGAIN", buf))
	{
	  poww.movegain[mg][0] = fread_number (fp);
	  poww.movegain[mg][1] = fread_number (fp);
	  poww.movegain[mg][2] = fread_number (fp);
	  poww.movegain[mg][3] = fread_number (fp);
	  mg++;
	  continue;
	}
      if (!str_cmp ("ASCENSION_REQ", buf))
	{
	  for(y = 0; y < 5; y ++)
	    poww.ascension_req[y] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("FORSAKE_REQ", buf))
	{
	  for(y = 0; y < 4; y ++)
	    poww.forsake_req[y] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REMORT_REQ", buf))
	{
	  int jk = 0;
	  for(jk = 0; jk < 8; jk++)
	    {
	      poww.remort_req[remort][jk] = fread_number (fp);
	    }
	  remort++;
	  continue;
	}
      if (!str_cmp ("ASCENDED_REMORT_REQ", buf))
	{
	  int jk = 0;
	  for (jk = 0; jk < 8; jk ++)
	    {
	      poww.ascended_remort_req[ascremort][jk] = fread_number (fp);
	    }
	  ascremort ++;
	  continue;
	}
      if (!str_cmp ("GUILD_COST", buf))
	{
	  int jjj = 0;
	  for (jjj = 0; jjj < 7; jjj++)
	    {
	      poww.guild_info[guild][jjj] = fread_number (fp);
	    }
	  guild++;
	  continue;
	}
      if (!str_cmp ("AUGMENT_COST", buf))
	{
	  int aug = 0;
	  for (aug = 0; aug < 7; aug++)
	    {
	      poww.augment_cost[augment][aug] = fread_number (fp);
	    }
	  augment++;
	  continue;
	}    
      if (!str_cmp ("TRANSFORM_IN_ROOM", buf))
	{
	  poww.transform_in_room = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("ASCTRANSFORM_IN_ROOM", buf))
	{
	  poww.asctransform_in_room = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("AUGMENT_REMORT_ONLY", buf))
	{
	  READ_YESNO (poww.augmentremortonly);
	  continue;
	}
      if (!str_cmp ("REMORT_IN_ROOM", buf))
        {
	   for (y = 0; y < 11; y ++)
	   poww.remort_in_room[y] = fread_number(fp);
	   continue;
         }
      if (!str_cmp ("ASCEND_REMORT_IN_ROOM", buf))
        {
	   for (y = 0; y < 11; y ++)
	   poww.ascend_remort_in_room[y] = fread_number (fp);
	   continue;
	}
      if (!str_cmp ("PRAC_LEVEL", buf))
	{
	  poww.low_range[i] = fread_number (fp);
	  poww.high_range[i] = fread_number (fp);
	  poww.learned[i] = str_dup (fread_word (fp));
	  i++;
	  continue;
	}
      if (!str_cmp ("ADDS_TO_STATS", buf))
	{
	  READ_YESNO (poww.add_to_stats);
	}
      if (!str_cmp ("MAXIMUM_REMORTS", buf))
	{
	  poww.max_remorts = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REMORT_LEVEL", buf))
	{
	  poww.remort_level = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("VAPORIZE_INVENTORY", buf))
	{
	  READ_YESNO (poww.vaporize_inv);
	}
      if (!str_cmp ("VAPORIZE_ENTIRE_EQU", buf))
	{
	  READ_YESNO (poww.vaporize_all);
	}
      if (!str_cmp ("GET_FROM_CORPSE_WHILE_IN_COMBAT", buf))
	{
	  READ_YESNO (poww.get_from_corpse_while_in_combat);
	}
      if (!str_cmp ("SHOW_RACE_SELECT_INFO", buf))
	{
	  READ_YESNO (poww.show_race_select_info);
	}
      if (!str_cmp ("GOOD_KILL_GOOD", buf))
	{
	  READ_YESNO (poww.good_kill_good);
	}
      if (!str_cmp ("EVIL_KILL_EVIL", buf))
	{
	  READ_YESNO (poww.evil_kill_evil);
	}

      if (!str_cmp ("MORTS_USE_COLOR_CHAT", buf))
	{
	  READ_YESNO (poww.morts_use_color_chat);
	}
      if (!str_cmp ("RESTORE_WHEN_LEVEL", buf))
	{
	  READ_YESNO (poww.restore_when_level);
	}
      if (!str_cmp ("PLAGUE_CHANCE", buf))
	{
	  poww.plague_chance = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("BEAMS_TO_ROOM", buf))
	{
	  poww.beams_to_room = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("ALL_NEWBIES_GOOD", buf))
	{
	  strcpy (buf, fread_word (fp));
	  if (UPPER (buf[0]) == 'Y')
	    poww.all_newbies_good = FALSE;
	  else
	    poww.all_newbies_good = TRUE;
	  continue;
	}
      if (!str_cmp ("LEVEL_TO_CHOOSE", buf))
	{
	  poww.level_to_choose = fread_number(fp);
	  continue;
	}
      if (!str_cmp ("CHOOSE_IN_ROOM", buf))
	{
	  poww.choose_in_room = fread_number(fp);
	  continue;
	}
      if (!str_cmp ("PREREQ_LEVEL", buf))
	{
	  poww.prereq = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("WEATHER_EFFECTS", buf))
	{
	  READ_YESNO (poww.weather);
	}
      if (!str_cmp ("PLAYER_PROMPT", buf))
	{
	  READ_YESNO (poww.player_prompt);
	}
      if (!str_cmp ("SKILL_LOSS_ON_MOB_DEATH", buf))
	{
	  READ_YESNO (poww.skill_loss_on_mob_death);
	}
      if (!str_cmp ("SKILL_LOSS_ON_PK_DEATH", buf))
	{
	  READ_YESNO (poww.skill_loss_on_pk_death);
	}
      if (!str_cmp ("SKILL_LOSS_AMOUNT", buf))
	{
	  poww.amount_lost_on_death = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("ARENA_ON", buf))
	{
	  READ_YESNO (poww.arena_on);
	}
      if (!str_cmp ("ARENA_GOOD_EVIL", buf))
	{
	  READ_YESNO (poww.arena_good_evil);
	}
      if (!str_cmp ("REQUIRED_VALIDATION", buf))
	{
	  READ_YESNO (poww.validation);
	}
      if (!str_cmp ("MUD_EMAIL_ADDRESS", buf))
	{
	  strcpy (poww.email_to, fread_word (fp));
	  continue;
	}
      if (!str_cmp ("EASIER_MOVE_STYLE", buf))
	{
	  READ_YESNO (poww.easier_moves);
	}
      if (!str_cmp ("MAX_PRACTICE_SKILLS", buf))
	{
	  poww.max_prac_skills = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("MOB_EXP", buf))
	{
	  poww.mob_exp = str_dup (fread_word (fp));
	  continue;
	}
      if (!str_cmp ("EXP_TO_LEVEL", buf))
	{
	  poww.exp_to_level = str_dup (fread_word (fp));
	  continue;
	}
      if (!str_cmp ("HIGHLEV_EXP_TO_LEVEL", buf))
	{
	  poww.highlev_exp_to_level = str_dup (fread_word (fp));
	  continue;
	}
      if (!str_cmp ("MAX_PRACTICE_SPELLS", buf))
	{
	  poww.max_prac_spells = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_1", buf))
	{
	  poww.newbie_object[0] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_2", buf))
	{
	  poww.newbie_object[1] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_3", buf))
	{
	  poww.newbie_object[2] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_4", buf))
	{
	  poww.newbie_object[3] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_5", buf))
	{
	  poww.newbie_object[4] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_6", buf))
	{
	  poww.newbie_object[5] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_7", buf))
	{
	  poww.newbie_object[6] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_8", buf))
	{
	  poww.newbie_object[7] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_9", buf))
	{
	  poww.newbie_object[8] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("NEWBIE_OBJECT_10", buf))
	{
	  poww.newbie_object[9] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("ALLOW_WHERE", buf))
	{
	  READ_YESNO (poww.allow_where);
	}
      if (!str_cmp ("CAN_SUMMON_MOBS", buf))
	{
	  READ_YESNO (poww.can_summon_mobs);
	}
      if (!str_cmp ("EVIL_CAN_CAST_SPELLS", buf))
	{
	  READ_YESNO (poww.evil_can_cast);
	}
      if (!str_cmp ("PERCENT_PRACTICES", buf))
	{
	  poww.practice_percent = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REPAIR_WEAPONS", buf))
	{
	  poww.repair_weapons[0] = fread_number (fp);
	  poww.repair_weapons[1] = fread_number (fp);
	  poww.repair_weapons[2] = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REPAIR_ARMORS", buf))
	{
	  poww.repair_armors = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("TINKER_DISCOUNT", buf))
	{
	  poww.tinker_discount = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("RESIZE_DIVISOR", buf))
	{
	  poww.resize_divisor = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("SPACE_NEEDED", buf))
	{
	  poww.space_needed = fread_number (fp);
	  if (fBootDb)
	    {
	      string_space = calloc (1, poww.space_needed);
	      top_string = string_space;
	    }
	  continue;
	}
      if (!str_cmp ("REMORT_BONUS_FIXED", buf))
	{
	  strcpy (buf, fread_word (fp));
	  if (UPPER (buf[0]) == 'Y')
	    poww.remort_bonus_fixed = TRUE;
	  else
	    poww.remort_bonus_fixed = FALSE;
	  continue;
	}
      if (!str_cmp ("EQUIP_IN_COMBAT", buf))
	{
	  READ_YESNO (poww.equip_in_combat);
	}
      if (!str_cmp ("REMOVE_WHILE_FIGHTING", buf))
	{
	  READ_YESNO (poww.remove_while_fighting);
	}
      if (!str_cmp ("SCORE_WHILE_FIGHTING", buf))
	{
	  READ_YESNO (poww.score_while_fighting);
	}
      if (!str_cmp ("AUTO_AUCTION", buf))
	{
	  READ_YESNO (poww.auto_auction);
	}
      if (!str_cmp ("FIGHT_SELF", buf))
	{
	  READ_YESNO (poww.can_fight_self);
	}
      if (!str_cmp ("LOOT_LEVEL", buf))
	{
	poww.loot_level=fread_number(fp);
	continue;
	}
      if (!str_cmp ("MAX_LEVEL", buf))
	{
	poww.max_level=fread_number(fp);
	continue;
	}
      if (!str_cmp ("AUCTION_LAG", buf))
	{
	  READ_YESNO (poww.auction_lag);
	}
      if (!str_cmp ("GIVE_WHILE_FIGHTING", buf))
	{
	  READ_YESNO (poww.give_while_fighting);
	}
      if (!str_cmp ("FLEE_CHANCE", buf))
	{
	  poww.flee_chance = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("PENALIZE_EVIL_IN_SUN", buf))
	{
	  READ_YESNO (poww.penalize_evil_in_sun);
	}
      if (!str_cmp ("BASH_SLIP_TO_TACKLE", buf))
	{
	  READ_YESNO (poww.bash_slip_to_tackle);
	}
      if (!str_cmp ("TACKLE_WITH_WEAPON", buf))
	{
	  READ_YESNO (poww.tackle_with_weapon);
	}
      if (!str_cmp ("TACKLE_PERSON_WITH_WEAPON", buf))
	{
	  READ_YESNO (poww.tackle_person_with_weapon);
	}
      if (!str_cmp ("ALLOW_REROLLS", buf))
	{
	  READ_YESNO (poww.allow_rerolls);
	}
      if (!str_cmp ("REGEN_MANA", buf))
	{
	  poww.base_mana_regen_bonus = fread_number (fp);
	  poww.med_mana_regen_bonus = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REGEN_HP", buf))
	{
	  poww.base_hp_regen = fread_number (fp);
	  poww.hp_bonus_resting = fread_number (fp);
	  poww.hp_bonus_sleeping = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("REGEN_MP", buf))
	{
	  poww.base_mp_regen = fread_number (fp);
	  poww.mp_bonus_resting = fread_number (fp);
	  poww.mp_bonus_sleeping = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("GROUNDFIGHT_STAND", buf))
	{
	  poww.groundfight_stand = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("FLURRY_MPS", buf))
	{
	  poww.flurry_mps = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("LEVEL_TO_SEE_NUMERIC_STATS", buf))
	{
	  poww.level_to_see_numeric_stats = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("HPS_FOR_CON", buf))
	{
	  int k;
	  for (k = 0; k < 36; k++)
	    {
	      con_app[k].hit = fread_number (fp);
	    }
	  continue;
	}
      if (!str_cmp ("PRACS_FOR_WIS", buf))
	{
	  int k;
	  for (k = 0; k < 36; k++)
	    {
	      wis_app[k].practice = fread_number (fp);
	    }
	  continue;
	}
      if (!str_cmp ("PRAC_INCREASE_FOR_INTELLIGENCE", buf))
	{
	  int k;
	  for (k = 0; k < 36; k++)
	    {
	      int_app[k].learn = fread_number (fp);
	    }
	  continue;
	}
      if (!str_cmp ("ARMOR_LEV", buf))
	{
	  poww.armor_lev_min[p] = fread_number (fp);
	  poww.armor_lev_max[p] = fread_number (fp);
	  poww.armor_lev_word[p] = fread_string_eol (fp);
	  p++;
	  continue;
	}
      if (!str_cmp ("GODS", buf)) {
	free_string(poww.gods);
	poww.gods = fread_string_eol(fp);
	continue;
	}
      if (!str_cmp ("SACNAMES", buf))
	{
	  poww.god_names[0] = str_dup (fread_word (fp));
	  poww.god_names[1] = str_dup (fread_word (fp));
	  poww.god_names[2] = str_dup (fread_word (fp));
	  poww.god_names[3] = str_dup (fread_word (fp));
	  poww.god_names[4] = str_dup (fread_word (fp));
	  continue;
	}
      fprintf (stderr, "Unknown read: %s.\n", buf);
      fread_to_eol (fp);
    }
  fclose (fp);
  return;
}

void 
read_race_info (void)
{
  FILE *fp;
  int i = 0;
  char wrd[256];
  if ((fp = fopen ("races.dat", "r")) == NULL)
    {
      fprintf (stderr, "Races.dat could not be opened!!\n");
      exit (7);
    }
  for (;;)
    {
      strcpy (wrd, fread_word (fp));
      if (!str_cmp ("END", wrd))
	break;
      if (!str_prefix ("#", wrd))
	{
	  fread_to_eol (fp);
	  continue;
	}
      strcpy (race_info[i].name, wrd);
      race_info[i].bonus[0] = fread_number (fp);
      race_info[i].bonus[1] = fread_number (fp);
      race_info[i].bonus[2] = fread_number (fp);
      race_info[i].bonus[3] = fread_number (fp);
      race_info[i].bonus[4] = fread_number (fp);
      race_info[i].limits[0] = fread_number (fp);
      race_info[i].limits[1] = fread_number (fp);
      race_info[i].limits[2] = fread_number (fp);
      race_info[i].limits[3] = fread_number (fp);
      race_info[i].limits[4] = fread_number (fp);
      race_info[i].start_room = fread_number (fp);
      race_info[i].max_skills = fread_number (fp);
      race_info[i].max_spells = fread_number (fp);
      race_info[i].max_profs = fread_number (fp);
      race_info[i].height_min = fread_number (fp);
      race_info[i].height_max = fread_number (fp);
      race_info[i].affect_bits = fread_number (fp);
      race_info[i].affect_bits2 = fread_number (fp);
      race_info[i].nat_augments = fread_number (fp);
      race_info[i].natarmor = fread_number (fp);
      race_info[i].regen_hit_percent = fread_number (fp);
      race_info[i].regen_move_percent = fread_number (fp);
      race_info[i].switch_aligns = fread_number (fp); 
      race_info[i].attack_spell_percent = fread_number (fp);
      race_info[i].combat_damage_bonus = fread_number (fp);
      race_info[i].backstab_percent = fread_number (fp);
      race_info[i].heal_percent = fread_number (fp);
      race_info[i].hps_bonus = fread_number (fp);
      race_info[i].mana_percent = fread_number (fp);
      race_info[i].nat_skills[0] = fread_number(fp);
      race_info[i].nat_skills[1] = fread_number(fp);
      race_info[i].nat_skills[2] = fread_number(fp);
      race_info[i].remort_race = fread_number (fp);
      i++;
    }
  fclose (fp);
  return;
}

void 
read_align_info (void)
{
  FILE *fp;
  int i = 0;
  char wrd[256];
  if ((fp = fopen ("align.dat", "r")) == NULL)
    {
      fprintf (stderr, "Align.dat could not be opened!!\n");
      exit (7);
    }
  for (;;)
    {
      strcpy (wrd, fread_word (fp));
      if (!str_cmp ("END", wrd))
	break;
      if (!str_prefix ("#", wrd))
	{
	  fread_to_eol (fp);
	  continue;
	}
      strcpy (align_info[i].name, wrd);
      align_info[i].bonus[0] = fread_number (fp);
      align_info[i].bonus[1] = fread_number (fp);
      align_info[i].bonus[2] = fread_number (fp);
      align_info[i].bonus[3] = fread_number (fp);
      align_info[i].bonus[4] = fread_number (fp);
      align_info[i].limits[0] = fread_number (fp);
      align_info[i].limits[1] = fread_number (fp);
      align_info[i].limits[2] = fread_number (fp);
      align_info[i].limits[3] = fread_number (fp);
      align_info[i].limits[4] = fread_number (fp);
      align_info[i].start_room = fread_number (fp);
      align_info[i].max_skills = fread_number (fp);
      align_info[i].max_spells = fread_number (fp);
      align_info[i].max_profs = fread_number (fp);
      align_info[i].height_min = fread_number (fp);
      align_info[i].height_max = fread_number (fp);
      align_info[i].affect_bits = fread_number (fp);
      align_info[i].affect_bits2 = fread_number (fp);
      align_info[i].nat_augments = fread_number (fp);
      align_info[i].natarmor = fread_number (fp);
      align_info[i].regen_hit_percent = fread_number (fp);
      align_info[i].regen_move_percent = fread_number (fp);
      align_info[i].switch_aligns = fread_number (fp); 
      align_info[i].attack_spell_percent = fread_number (fp);
      align_info[i].combat_damage_bonus = fread_number (fp);
      align_info[i].backstab_percent = fread_number (fp);
      align_info[i].heal_percent = fread_number (fp);
      align_info[i].hps_bonus = fread_number (fp);
      align_info[i].mana_percent = fread_number (fp);
      align_info[i].nat_skills[0] = fread_number(fp);
      align_info[i].nat_skills[1] = fread_number(fp);
      align_info[i].nat_skills[2] = fread_number(fp); 
      i++;
    }
  fclose (fp);
  return;
}
void 
reload_power_dat (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("z_reload_power_data", reload_power_dat, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Reloads the power.dat data from disk.")
    read_power_dat ();
  read_align_info ();
  read_race_info ();
  return;
}
