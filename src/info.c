#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void
do_forgecost (CHAR_DATA * ch, char *argy)
{
 DEFINE_COMMAND ("forgecost", do_forgecost, POSITION_DEAD, 1, LOG_NORMAL, "this command brings up out todo list in a command insted of a help file.")
    do_help (ch, "forgecost");
} // Kilith 2005

void
do_anon (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("anonymous", do_anon, POSITION_DEAD, 0, LOG_NORMAL, "This command toggles whether or not your level shows up on the who list.")

    if (ch->pcdata->show_lev)
    {
      send_to_char ("Your level will not be shown in the who list from now on.\n\r", ch);
      ch->pcdata->show_lev = FALSE;
      return;
    }
  send_to_char ("Your level will now be shown in the who list for others to see.\n\r", ch);
  ch->pcdata->show_lev = TRUE;
  return;
}

void
do_levels (CHAR_DATA * ch, char *argy)
{
  int i, a, b;
  char buffr[256];
  char arg1[SML_LENGTH];
  DEFINE_COMMAND ("levels", do_levels, POSITION_DEAD, 0, LOG_NORMAL, "This command shows exp needed for the level range specified.")

    buffr[0] = '\0';
  argy = one_argy (argy, arg1);
  if (!is_number (argy) || !is_number (arg1))
    {
      send_to_char ("Syntax is \"level <min> <max>\"\n\r", ch);
      return;
    }
  a = atoi (arg1);
  b = atoi (argy);
  if (a >= b || a < 1 || a > 90 || b < 2 || b > 90)
    {
      send_to_char ("Invalid range.\n\r", ch);
      return;
    }
  for (i = a; i < b; i++)
    {
      sprintf (buffr, "Level %d Exp: %ld\n\r", i + 1, FIND_EXP (i, ch->race));
      send_to_char (buffr, ch);
    }
  return;
}


void
do_reload_help (CHAR_DATA * ch, char *argy)
{
  FILE *fp;
  char word[500];
  HELP_DATA *hnx;
  HELP_DATA *hc;
  DEFINE_COMMAND ("rld_help", do_reload_help, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command reloads the helpfiles from disk.")

    if ((fp = fopen ("help.are", "r")) == NULL)
    {
      send_to_char ("Unable to open the helpfile...", ch);
      return;
    }
  for (hc = help_first; hc != NULL; hc = hnx)
    {
      hnx = hc->next;
      free_string (hc->keyword);
      free_string (hc->text);
    }
  help_last = NULL;
  help_first = NULL;
  top_help = 0;
  word[0] = '\0';
  while (str_cmp ("#HELPS", word))
    strcpy (word, fread_word (fp));
  load_helps (fp);
  send_to_char ("Successfully reloaded.\n\r", ch);
  fclose (fp);
  return;
}

void
do_help (CHAR_DATA * ch, char *argy)
{
  char argall[SML_LENGTH];
  char argone[SML_LENGTH];
  HELP_DATA *pHelp;
  DEFINE_COMMAND ("help", do_help, POSITION_DEAD, 0, LOG_NORMAL, "This is the basic help command.  Syntax is 'help' or 'help <topic>'.")

    if (argy[0] == '\0')
      {
       send_to_char("Type help <word> for specific help. Typing commands will give you a list of all the commands available to you.\n\r", ch);
       return;
      }
    

/*
   * Tricky argy handling so 'help a b' doesn't match a.
 */
  argall[0] = '\0';
  while (argy[0] != '\0')
    {
      argy = one_argy (argy, argone);
      if (argall[0] != '\0')
        strcat (argall, " ");
      strcat (argall, argone);
    }
  for (pHelp = help_first; pHelp && pHelp != NULL; pHelp = pHelp->next)
    {
      if (pHelp->level > LEVEL (ch))
        continue;
      if (is_name (argall, pHelp->keyword))
        {
	  int i;
	  char hugebuf_o[30000] = "\0";
	  char buff[500] = "\0";
	  int offset = (38- (strlen(pHelp->keyword))/2);
	  if (offset > 35) offset = 35;
	  if (offset < 0) offset = 0;
          if (pHelp->level >= 0 && str_cmp (argall, "imotd"))
            {
              send_to_char ("\n\r", ch);
            }
          /*
             * Strip leading '.' to allow initial blanks.
	     */
	  strcat(hugebuf_o, "\n\r");
	  strcat(hugebuf_o, decompress("{C}[{c}+{C}]{c}---------------------------------------------------------------------{C}[{c}+{C}]{C}\n\r"));
	  for (i=0; i <offset; i++)
	    strcat(hugebuf_o, " ");

	  
	  sprintf(buff, "%s\n\r", pHelp->keyword);
	  strcat(hugebuf_o, buff);
	  strcat(hugebuf_o, decompress("{C}[{c}+{C}]{c}---------------------------------------------------------------------{C}[{c}+{C}]{C}\n\r{w}"));
	  if (pHelp->text[0] == '.')
	    strcat(hugebuf_o, (decompress (pHelp->text)));
	  else
	    strcat(hugebuf_o, (decompress (pHelp->text)));
	  page_to_char(hugebuf_o, ch);
          return;
	}
    }
  send_to_char ("No help on that word.\n\r", ch);
  return;
}
void
do_help_two (CHAR_DATA * ch, char *argy)
{
  char argall[SML_LENGTH];
  char argone[SML_LENGTH];
  char j[50];
  HELP_DATA *pHelp;

    if (argy[0] == '\0')
      {
	sprintf(j,"%c",28);
	write_to_buffer(ch->desc,j,0);
      }
    

/*
   * Tricky argy handling so 'help a b' doesn't match a.
 */
  argall[0] = '\0';
  while (argy[0] != '\0')
    {
      argy = one_argy (argy, argone);
      if (argall[0] != '\0')
        strcat (argall, " ");
      strcat (argall, argone);
    }
  for (pHelp = help_first; pHelp && pHelp != NULL; pHelp = pHelp->next)
    {
      if (pHelp->level > LEVEL (ch))
        continue;
      if (is_name (argall, pHelp->keyword))
        {
	  char hugebuf_o[30000] = "\0";
         
          /*
             * Strip leading '.' to allow initial blanks.
	     */
	  if (pHelp->text[0] == '.')
	    strcat(hugebuf_o, (decompress (pHelp->text)));
	  else
	    strcat(hugebuf_o, (decompress (pHelp->text)));
	  page_to_char(hugebuf_o, ch);
          return;
	}
    }
  send_to_char ("There is no help file for that skill or spell.\n\r", ch);
  return;
}

void
do_who (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *wch;
  int pass;
  DESCRIPTOR_DATA *d;
  int vis;
  int ii;
  int nMatch;
  bool tempbool;
  int gMatch;
  int align;
  bool namey;
  char doe[STD_LENGTH];
  bool clanny;
  bool battle;
  bool hc;
  DEFINE_COMMAND ("who", do_who, POSITION_DEAD, 0, LOG_NORMAL, "This command shows who is playing and visible to you at the moment.")

    ii = clan_number (ch);
  align = 0;
  nMatch = 0;
  clanny = FALSE;
  battle = FALSE;
  hc = FALSE;
  gMatch = 0;
  vis = 0;
  hugebuf_o[0] = '\0';
  sprintf (hugebuf_o, "                       \x1b[1;37m-\x1b[1;30m=\x1b[1;37m The Lands of Chaos \x1b[1;30m=\x1b[1;37m-\n\x1b[1;30m-----------------------------------------------------------------------------\x1B[0;37m\n\r"); 
  namey = FALSE;
 
  if (LEVEL(ch) < 20 && ch->pcdata->remort_times == 0) /* Stop spies */
    return;

  /* In general, gods see all align = -2 means see all, align = -1 = see imms only, align = 0-NUM_ALIGN-1 means see that align only */

  if (LEVEL(ch) >=MAX_LEVEL)
    {
      align = -2;
      if (!str_cmp (argy, "clan"))
        {
          clanny = TRUE;
          if (ii < 1)
            {
              send_to_char ("You aren't in a clan!\n\r", ch);
              return;
            }
        }
      else if (!str_cmp (argy, "battle") || !str_cmp (argy, "battleground"))
	battle = TRUE;
      else if (!str_cmp (argy, "god") || !str_cmp (argy, "immortal"))
        align = -1; /* Show imms only */
      else if (is_number (argy))
	{
	  align = atoi (argy);
	  if (align < 0 || align >= NUM_ALIGN)
	    align = -2;
	}
      else if (argy[0] != '\0')
        {
          argy[0] = UPPER (argy[0]);
          namey = TRUE;
        }
      else align = -2;
    }
  else if (LEVEL(ch) > IMM_LEVEL && LEVEL(ch) <MAX_LEVEL)
    align = -1;
  else if (LEVEL(ch) < IMM_LEVEL)
    {
      align = ALIGN(ch);
      if (align < 0 || align >= NUM_ALIGN) return;
      if (argy[0] != '\0' || argy != "")
	{
	  if (!str_cmp (argy, "clan"))
	    {
	      clanny = TRUE;
	      if (ii < 1)
		{
		  send_to_char ("You aren't in a clan!\n\r", ch);
		  return;
		}
	    }
	  else if (!str_cmp (argy, "battle") || !str_cmp (argy, "battleground"))
	    battle = TRUE; /* Show only who is in battle */
	  else if (!str_cmp (argy, "god") || !str_cmp (argy, "immortal"))
	    align = -1;
	  else if (argy[0] != '\0')
	    {
	      argy[0] = UPPER (argy[0]);
	      namey = TRUE;
	    }
	}
    }
  tempbool = FALSE;
  for (pass = -1; pass < NUM_ALIGN; pass++)
    {
      if (!tempbool && pass != -1)
        {
          tempbool = TRUE;
	  if (LEVEL(ch) < MAX_LEVEL)
	    sprintf (hugebuf_o + strlen (hugebuf_o), "\x1b[1;32mAl \x1b[1;34mLv \x1b[1;31mRm\x1b[1;37m  Elems \x1B[0;37m- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\x1B[37;0m\n\r");
	  else
	    sprintf (hugebuf_o + strlen (hugebuf_o), "\x1B[0;36mAl Lv Rm  Elems - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\x1B[37;0m\n\r");
	  /*

	   {
              int j = 0;
		int len = strlen(align_info[pass].name);
		int flex = 38-(7+len/2);
		char buff[50];
		char bufg[50];
		sprintf(buff, "\x1b[1;34m");
		sprintf(bufg, "\x1b[1;34m");
		
		for (j = 0; j < flex; j++)
		  strcat(buff, "-");
		 for (j = 0; j < (flex-1); j++)
		  strcat(bufg, "-");
		 if (!IS_SET(len, 1))
		   strcat(bufg, "-");
		 
		 sprintf (hugebuf_o + strlen (hugebuf_o), "%s\x1b[0;37m[ \x1b[1;37m%s Characters\x1b[0;37m ]%s\x1B[0;37m\n\r", buff, capitalize(align_info(pass).name), bufg);
	      }
	    */
	  
          }
      for (d = descriptor_list; d != NULL; d = d->next)
        {
          wch = NULL;
          if (d->character == NULL)
            continue;
          if (IS_MOB (d->character))
            continue;
          if (d->character->desc == NULL)
            continue;
          if ((d->connected != CON_PLAYING
               && d->connected != CON_AEDITOR
               && d->connected != CON_REDITOR
               && d->connected != CON_CEDITOR
               && d->connected != CON_OEDITOR
               && d->connected != CON_MEDITOR
               && d->connected != CON_FSEDITOR
               && d->connected != CON_SOCEDIT
               && d->connected != CON_GODMENU
               && d->connected != CON_SCRIPTEDITOR
               && d->connected != CON_SHELL
	       && d->connected != CON_PEDITOR
	       && d->connected != CON_SEDITOR)
               || !can_see (ch, d->character))
             continue;
          if (d->connected != CON_PLAYING && !IS_IMMORTAL (ch))     continue;
          wch = (d->original != NULL) ? d->original : d->character;
          if (clanny && clan_number (wch) != ii)  continue;
          if (namey && str_prefix (capitalize (argy), NAME (wch))) continue;
          if (battle && !IN_BATTLE (wch)) continue;
          if ((LEVEL (wch) > IMM_LEVEL) == (pass != -1)) continue;
          if (LEVEL (wch) <= IMM_LEVEL && (pass != ALIGN(wch) || align == -1)) continue;
          if (align >= 0)
          {
              if (!alliance[align][ALIGN(wch)] && LEVEL(wch) < MAX_LEVEL && LEVEL(ch) < MAX_LEVEL) continue;
              if (LEVEL (ch) < MAX_LEVEL && DIFF_ALIGN(ch, wch) && LEVEL(wch) < IMM_LEVEL) continue;
              if (LEVEL (wch) < MAX_LEVEL && LEVEL (wch) > IMM_LEVEL && LEVEL (ch) < IMM_LEVEL) continue;
              if (LEVEL(ch) == MAX_LEVEL && align!=wch->pcdata->alignment
                  && LEVEL(wch) < IMM_LEVEL) continue;
 	  }
          nMatch++;
          doe[0] = '\0';
          if (LEVEL (wch) > 99)
            {
              if (wch->pcdata->wizinvis == 0)
                gMatch++;
              if (d->connected != CON_PLAYING)
		{
		  switch (d->connected)
		  {
		    case CON_AEDITOR:
                      sprintf (doe, "\x1B[1;36m   Area Editor   ");
		      break;
		    case CON_REDITOR:
                      sprintf (doe, "\x1B[1;36m   Room Editor   ");
		      break;
		    case CON_MEDITOR:
                      sprintf (doe, "\x1B[1;36m    Mob Editor   ");
		      break;
		    case CON_OEDITOR:
                      sprintf (doe, "\x1B[1;36m  Object Editor ");
		      break;
		    case CON_CEDITOR:
                      sprintf (doe, "\x1B[1;36m   Clan Editor   ");
		      break;
		    case CON_FSEDITOR:
                      sprintf (doe, "\x1B[1;36m    FS Editor    ");
		      break;
		    case CON_PEDITOR:
                      sprintf (doe, "\x1B[1;36m  Player Editor  ");
		      break;
		    case CON_SEDITOR:
                      sprintf (doe, "\x1B[1;36m   Spell Editor  ");
		      break;
		    case CON_GODMENU:
                      sprintf (doe, "\x1B[1;36m     God Menu    ");
		      break;
		    case CON_SOCEDIT:
                      sprintf (doe, "\x1B[1;36m  Social Editor  ");
		      break;
		    case CON_SCRIPTEDITOR:
                      sprintf (doe, "\x1B[1;36m  Script Editor  ");
		      break;
		    case CON_SHELL:
                      sprintf (doe, "\x1B[1;36m       Shell     ");
		      break;
		    default:
                      sprintf (doe, "\x1B[1;36m     Building    ");
		      break;
		  }
		}
              else
                {
                  switch (LEVEL (wch))
                    {
                    default:
                      sprintf (doe, "\x1B[1;33m  Builder  ");
                      break;
                    case TOP_LEVEL:

/*   COMMENTED OUT --------------------------------------------------------------------
 */               /*       if (!str_cmp(NAME(wch), "Tonik"))
			 sprintf(doe, "\x1b[1;35m -* A Kinder Gentler *-");
		      else if (!str_cmp(NAME(wch), "Distance"))
			 sprintf(doe, "\x1b[1;35m -* Special Enforcer *-");
		      else if (!str_cmp(NAME(wch), "Valor"))
			 sprintf(doe, "\x1B[1;37m -* Map Admin *-");
		      else if (!str_cmp(NAME(wch), "Structure"))
			 sprintf(doe, "\x1B[1;37m -* Outline Admin *-");
			else */
/* 
------------------------------------------------------------------------------------- */
                        sprintf (doe, "\x1b[1;35m   -* Admin *-   ");
                      break;
                      case 111:
                        sprintf (doe, "\x1B[1;34m -*System Admin*-");
                          break;
                    }
                }
              if (wch->pcdata->quiet == 1)
                sprintf (doe, "\x1B[1;35m- Do Not Disturb -");
              if (wch->pcdata->quiet == 2)
                sprintf (doe, "\x1B[1;34m- Busy Mode - ");
            }
          {
            if (IS_PLAYER (ch) && LEVEL (ch) < IMM_LEVEL && !IS_MOB (wch) && !DIFF_ALIGN(ch, wch))
              vis++;
/*	    
            if (ii > 0 && LEVEL(ch) < MAX_LEVEL)
              {
                if (clan_number (wch) == ii)
                  {
                    sprintf (hugebuf_o + strlen (hugebuf_o), "<C>");
                  }
                else
                  sprintf (hugebuf_o + strlen (hugebuf_o), "   ");
              }  TOOK THIS OUT */
             // if (LEVEL (ch) <= 100 && LEVEL (wch) >= 110 && IS_SET (ch->act, PLR_HOLYLIGHT))
            if (LEVEL (ch) <= 111 && LEVEL (wch) <= 99)
              {
                bool alignshard = FALSE;
                if (IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_GOOD)
                  || IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EVIL))
                    alignshard = TRUE;
		/* ------------------------------------------------------------- */
		/*  		 Display WHO LIST for morts here		 */
		/* ------------------------------------------------------------- */
                sprintf (hugebuf_o + strlen (hugebuf_o),
/*           A L, R,            f a w e -             s - + H                                  g                      a n  t i w*/
"\x1B[1;30m[%d%3d%3d\x1B[1;30m][%s%s%s%s%s\x1B[1;30m] %s%s%s%s\x1B[37;0m \x1B[1;37m[\x1B[1;37m%s\x1B[1;37m]\x1B[0;37m %s%s %s%s%s\x1B[0;37m\n\r", 
			 ALIGN(wch),  LEVEL (wch), 
			 wch->pcdata->remort_times,
  (IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_FIRE) ? "\x1b[1;31m-" : " "),
  (IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_AIR) ? "\x1b[1;36m-" : " "),
  (IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_WATER) ? "\x1b[1;34m-" : " "),
  (IS_SET(wch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EARTH) ? "\x1b[1;32m-" : " "),
  (alignshard ? "\x1b[1;37m-" : " "),
			 (IS_SET (wch->pcdata->act2, PLR_SILENCE) ? " s" : " "),
                         (wch->pcdata->bgf ? "-" : " "), (IN_BATTLE (wch) ? "+" : " "),
			 (IS_SET(wch->pcdata->act2, PLR_HARDCORE) ? "\x1b[1;31mH" : " "),

                         //get_main_elemental_align(wch),
			 get_main_guild(wch),
                         wch->timer > 49 ? "\x1B[1;37m<AFK> \x1B[0;37m" : "", NAME (wch),
			 wch->pcdata->title,
			 (IS_AFFECTED (wch, AFF_INVISIBLE)) ? " \x1b[0;36m(\x1b[1;30mInvis\x1b[0;36m)" : "",
                         (wch->pcdata->wizinvis != 0 ? " \x1b[1;30m(\x1b[1;37mInvis\x1b[1;30m)" : ""));
              }
            else
              {
                if (doe[0] == '\0')
                  {
                    if (!wch->pcdata->show_lev)
                      sprintf (doe, "\x1B[1;34m--");
                    else
                      sprintf (doe, "\x1b[1;37mLevel %2d",
                	LEVEL (wch));  
		}

		/* ------------------------------------------------------------- */
		/*  		 Display WHO LIST for imms here */
		/* ------------------------------------------------------------- */
                sprintf (hugebuf_o + strlen (hugebuf_o),
/*                                    L                         G                       H            A N  T I W*/
                         "\x1B[1;30m[ %s \x1B[1;30m] [\x1B[1;37m%s\x1B[1;30m] \x1B[1;31m%s\x1b[0;37m %s%s %s%s%s%s\x1B[37;0m\n\r",
                         doe,
			 //get_main_elemental_align(wch),
			 //get_main_elemental_align(wch) != "" ? strcat(get_main_elemental_align(wch),"\x1B[1;30m-") : "",
			 get_main_guild(wch),
			 (IS_SET(wch->pcdata->act2, PLR_HARDCORE) ? "\x1b[1;31mH" : " "),
                         wch->timer > 49 ? "\x1B[1;37m<AFK> \x1B[0;37m" : "", NAME (wch),   
			 wch->pcdata->title, 
                         (IS_AFFECTED (wch, AFF_HIDE)) ? " \x1b[1;30m(Hidden)\x1b[0;37m" : "",
                         (IS_AFFECTED (wch, AFF_INVISIBLE)) ? " \x1b[1;30m(\x1b[0;36mInvis\x1b[0;30m)" : "",
			 (wch->pcdata->wizinvis != 0 ? " \x1b[1;30m(\x1b[0;36mWizInvis\x1b[1;30m)" : ""));

		
              }
          }
        }
      /*end Pass */
    }
  sprintf (hugebuf_o + strlen (hugebuf_o), "\x1b[1;30m-----------------------------------------------------------------------------\x1B[0;37m\n\r", 4);
  sprintf (hugebuf_o + strlen (hugebuf_o), "         \x1b[1;30m There %s \x1b[1;37m%d\x1b[1;30m player%s currently visible to you at this time.\n\r" ,
            (LEVEL(ch) > IMM_LEVEL ? nMatch : vis)> 1 ? "are" : "is", (LEVEL(ch) > IMM_LEVEL ? nMatch : vis),
            (LEVEL(ch) > IMM_LEVEL ? nMatch : vis) > 1 ? "s" : "", max_connected,
	   max_connected > 1 ? "s" : "");
  page_to_char (hugebuf_o, ch);
  return;
}


/* Method to return a colourful short form of the character's main guild
 * for use in the who list. send a pointer to a char, get a string
 */
char* get_main_guild (CHAR_DATA * ch)
{
  char* buf;

  if (is_member(ch, GUILD_MYSTIC) && is_member(ch, GUILD_HEALER)) buf = "\x1b[0;32mC\x1b[1;32mL\x1b[0;32mE";
  else if (is_member(ch, GUILD_MONK) && is_member(ch, GUILD_HEALER)) buf = "\x1b[1;37mM\x1b[1;36mN\x1b[1;34mK";
  else if (is_member(ch, GUILD_WIZARD) && is_member(ch, GUILD_WARRIOR)) buf="\x1b[1;31mW\x1b[0;31mL\x1b[1;30mK";
  else if (is_member(ch, GUILD_ROGUE) && is_member(ch, GUILD_THIEFG)) buf = "\x1b[1;30mR\x1b[0;37mO\x1b[1;37mG";
  else if (is_member(ch, GUILD_CONJURER) && is_member(ch, GUILD_WIZARD)) buf = "\x1b[1;34mS\x1b[0;34mO\x1b[1;36mR";
  else if (is_member(ch, GUILD_TINKER) && is_member(ch, GUILD_WARRIOR)) buf ="\x1b[0;37mWA\x1b[1;37mR";
  else if (is_member(ch, GUILD_NECROMANCER) && is_member(ch, GUILD_CONJURER)) buf ="\x1b[0;31mN\x1b[1;30mE\x1b[0;31mC";
  else if (is_member(ch, GUILD_HEALER) && is_member(ch, GUILD_WARRIOR)) buf = "\x1b[1;36mP\x1b[0;36mAL"; 
  else buf = "\x1b[0;31mA\x1b[1;31mD\x1b[0;31mV";

  return buf;
}

/* method to return colorful short form of element they are aligned to */

char* get_main_elemental_align(CHAR_DATA * ch) {

  char* buf;

  if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_FIRE)) buf = "\x1B[1;31mF\x1B[0;37m-";
  else if (IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_WATER)) buf = "\x1B[1;34mW\x1B[0;37m-";
  else if (IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_AIR)) buf = "\x1B[1;36mA\x1B[0;37m-";
  else if (IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EARTH)) buf = "\x1B[1;32mE\x1B[0;37m-";
  else if (IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_GOOD)) buf = "\x1B[1;29mG\x1B[0;37m-";
  else if (IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EVIL)) buf = "\x1B[1;30mE\x1B[0;37m-";
  else buf = "";

  return buf;
}


char *const day_name[] =
{
  "Monday", "Tuesday", "Wednesday", "Thursday",
  "Friday", "Saturday", "Sunday"
}

 ;
char *const month_name[] =
{
  "Winter", "Winter", "Midwinter", "Midwinter",
  "Early Spring", "Spring", "Spring",
  "Late Spring", "Early Summer", "Summer",
  "Midsummer", "High Summer", "Late Summer", "Early Autumn",
  "Autumn", "Mid-Autumn", "Late Autumn", "Aturion Twilight",
  "Aturion Twilight", "The Frigid Moon", "Early Winter"
}

 ;
void
do_time (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char *suf;
  int day;
  DEFINE_COMMAND ("time", do_time, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you the current time.")

    day = time_info.day + 1;
  if (day > 4 && day < 20)
    suf = "th";
  else if (day % 10 == 1)
    suf = "st";
  else if (day % 10 == 2)
    suf = "nd";
  else if (day % 10 == 3)
    suf = "rd";
  else
    suf = "th";
  sprintf (buf,
  "It is %d o'clock %s on the day of %s.\n\r(The %d%s of %s. Year: %d)\n\r",
           (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
           time_info.hour >= 12 ? "pm" : "am",
           day_name[day % 7],
           day, suf,
           month_name[time_info.month], time_info.year);
  send_to_char (buf, ch);
  return;
}

int
cur_temp (CHAR_DATA * ch)
{
  int temp_adj;
  int rweth;
  rweth = weather_info.temperature;
  temp_adj = 0;
  if (!ch->in_room || !ch->in_room->area)
    return 60;
  switch (ch->in_room->sector_type)
    {
    case SECT_LAVA:
      temp_adj = 110;
      break; 
    case SECT_DESERT:
    case SECT_WASTELAND:
      temp_adj = 60;
      break;
    case SECT_TROPICAL:
    case SECT_SWAMP:
    case SECT_MARSH:
      temp_adj = 50;
      break;
    case SECT_SNOW:
    case SECT_ICE:
    case SECT_ARCTIC:
      temp_adj = -30;
      break;
    case SECT_INSIDE:
      temp_adj = 15;
      break;
    case SECT_MOUNTAIN:
    case SECT_HILLS:
    case SECT_ROCKY:
      temp_adj = -17;
      break;
    case SECT_AIR:
    case SECT_CANYON:
    case SECT_ASTRAL:
    case SECT_BRUSH:
      temp_adj = -15;
      break;
    }
  if (ch->in_room->sector_type >= SECT_CAVE)
    temp_adj = -10;
  rweth += temp_adj;
  return rweth;
}

// Some Weather crap borrowed from other codebases */

void
do_weather (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char descr[STD_LENGTH];
  short wind;
  int temp_adj, sky, wt;
  static char *const sky_look[] =
  {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning",
    "thick with fog",
    "slightly sunny",
    "slightly overcast",
    "scattered with a few flakes",
    "filled with flakes",
    "a blizzard of white",
    "thick with fog",
    "not visible underwater.",
    "stormy"
  }
   ;
  static char *const wind_tmp[4] =
  {
    "cold", "cool", "warm", "hot"
  }
   ;
  DEFINE_COMMAND ("weather", do_weather, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to see the weather (outdoors).")

    wind = weather_info.windspeed;
  sky = weather_info.sky;
  temp_adj = 0;
  do_time (ch, "");
  switch (ch->in_room->sector_type)
    {
    case SECT_LAVA:
      temp_adj = 110;
      break; 
    case SECT_DESERT:
    case SECT_WASTELAND:
      temp_adj = 60;
      break;
    case SECT_TROPICAL:
    case SECT_SWAMP:
    case SECT_MARSH:
      temp_adj = 50;
      break;
    case SECT_SNOW:
    case SECT_ICE:
    case SECT_ARCTIC:
      temp_adj = -30;
      break;
    case SECT_INSIDE:
      temp_adj = 15;
      break;
    case SECT_MOUNTAIN:
    case SECT_HILLS:
    case SECT_ROCKY:
      temp_adj = -17;
      break;
    case SECT_AIR:
    case SECT_CANYON:
    case SECT_ASTRAL:
    case SECT_BRUSH:
      temp_adj = -15;
      break;
    }
  if (temp_adj + weather_info.temperature > 90)
    wt = 3;
  else if (temp_adj + weather_info.temperature > 75)
    wt = 2;
  else if (temp_adj + weather_info.temperature > 50)
    wt = 1;
  else
    wt = 0;
  if (!IS_OUTSIDE (ch) || ch->in_room->sector_type >= SECT_CAVE)
    {
      send_to_char ("You can only see the weather from the outdoors.\n\r", ch);
      return;
    }
  descr[0] = '\0';
  sprintf (buf, "The sky is %s ",
           ((time_info.month <= 4 || time_info.month >= 17)
            && (temp_adj + weather_info.temperature <= 32)) ?
           sky_look[sky + 5] : sky_look[sky]);
  strcat (descr, buf);
  if (wt != -1)
    {
      sprintf (buf, "and a %s %sward %s blows.\n\r",
               wind_tmp[wt],
               dir_name[abs (weather_info.winddir) % 3],
               wind < 10 ? "breeze" :
               wind < 20 ? "wind" :
               wind < 30 ? "gust" : "torrent");
      strcat (descr, buf);
    }
  sprintf (buf, "It is %d degrees right now.\n\r",
           (temp_adj + weather_info.temperature));
  strcat (descr, buf);
/*if ( IS_IMMORTAL( ch ) )
   {
   sprintf( buf, "(temp: %d, windspd: %d, wind dir: %d, sky: %d, mmhg: %d, change: %d)\n\r",
   (temp_adj+weather_info.temperature),
   weather_info.windspeed,
   weather_info.winddir,
   weather_info.sky,
   weather_info.mmhg,
   weather_info.change );
   strcat( descr, buf );
   }
 */
  send_to_char (descr, ch);
  return;
}
