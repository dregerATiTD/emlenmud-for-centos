#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

#define R(cmd) cmd(NULL,"");

/* Commands like "north" etc... must go near the bottom... */

void 
scanthru_commands (void)
{
/*  R (do_towers) NOT USED ATM */
  R(view_scriptassoc)
  R(do_quests)
  R(do_clan_notify)
  R (do_forsake)
    R (do_random_split)
    R (do_forgecost)
    R (do_ascend)
    R (do_spico_help)
    R (do_shell)
    R (do_startup_time)
    R (flags)
    R (debug)
    R (do_enchant)
    R (do_mapspam)
    R (do_mapping)
    R (do_noibm)
    R (do_unload)
    R (do_unenchant)
    R (do_alliance)
    R (do_viewmobs)
    R (do_viewplayers)
    R (online_coding)
    R (do_raceinfo)
    R (do_alignmentinfo)
    R (list_code)
    R (do_new_prereq)
    R (do_weave)
    R (do_stitch)
    R (do_lore)
    R (do_mine)
    R (do_skin)
    R (do_tan)
    R (do_build)
    R (do_chop)
    R (list_triggers)
    R (script_editor)
    R (do_steal)
    R (do_patch)
    R (do_fix)
    /* R (do_detrap) */
    R (do_mat)
    R (do_soc_edit)
    R (z_areachange)
    R (do_setmiw)
    R (do_where)
    R (clean_blank_rooms)
    R (relocate_rooms)
    R (relocate_mobs)
    R (relocate_objs)
    R (do_idea)
    R (do_augment)
    R (show_affect_list)
    R (show_affect_list2)
    R (read_score)
    R (do_commands)
    R (do_ocopy)
    R (do_mcopy)
    R (do_rcopy)
    R (decode)
    R (do_unlearn)
    R (compare)
    R (do_dam)
/*
    R (do_clanstore)
    R (do_clanunstore)
*/
    R (disengage)
    R (do_repop)
    R (do_cadd)
    R (do_cremove)
    R (do_clan_quit)
    R (do_climb)
    R (clean_oldchars)
    R (clean_up)
    R (p_refresh)
    R (do_battleground)
    R (reload_pbase)
    R (do_tauction)
    R (do_bid)
    R (do_group_consider)
    R (do_guild)
    R (do_preset)
    R (do_sedit)
    R (do_notify)
    R (do_beamin)
    R (do_beamout)
    R (no_spam)
    R (replace)
    R (spam)
    R (do_disconnect)
    R (do_echo)
    R (do_recho)
    R (do_goto)
    R (do_rstat)
    R (do_ostat)
    R (do_mstat)
    R (do_mfind)
    R (do_ofind)
    R (grab_hostnames)
    R (do_rfind)
    R (do_mwhere)
    R (do_pstat)
    R (do_reboo)
    R (do_reboot)
    R (do_snoop)
    R (do_switch)
    R (do_list)
    R (do_sell)
    R (do_buy)
    R (do_value)
    R (do_resize)
    R (do_repair)
    R (do_return)
    R (do_mload)
    R (do_oload)
    R (do_purge)
    R (do_advance)
    R (do_restore)
    R (do_freeze)
    R (do_noclan)
    R (logsearch)
    R (do_lamer)
    R (do_shitlist)
    R (do_log)
    R (do_silence)
    R (do_peace)
    R (do_ban)
    R (do_nban)
    R (do_allow)
    R (do_wizlock)
    R (do_slookup)
    R (do_award_skill)
    R (do_sset)
    R (do_mset)
    R (do_oset)
    R (do_pset)
    R (do_users)
    R (do_force)
    R (do_invis)
    R (do_holywalk)
    R (do_holylag)
    R (do_holypeace)
    R (do_holylight)
    R (do_owhere)
    R (do_count)
    R (do_noaffect)
    /* R (do_trapset) */
    R (do_setlevels)
    R (do_arena)
    R (do_bet)
    R (do_brew)
    R (do_forge)
    R (do_acceptc)
    R (do_decline)
    R (do_challenge)
    R (do_set_fighters)
    R (reload_power_dat)
    R (do_pedit)
	R (do_order)
    R (do_clear)
    R (do_fly)
    R (do_land)
    R (do_enter)
    R (do_run)
    R (do_open)
    R (do_close)
    R (do_lock)
    R (do_unlock)
    R (do_pick)
    R (do_sneak)
    R (do_hide)
    R (do_visible)
    R (do_brief)
    R (do_blank)
    R (do_break)
    R (do_glance)
    R (do_scan)
    R (do_examine)
    R (do_dir)
    R (do_purse)
    R (do_equipment)
    R (do_vequipment)
    R (do_finger)
    R (do_learn)
    R (do_report)
    R (do_skill)
    R (do_practice)
    R (do_prereqs)
    R (do_password)
    R (do_socials)
    R (do_channels)
    R (do_spells)
    R (do_slist)
    R (do_search)
    //R (do_anon)
    R (do_levels)
    R (do_reload_help)
    R (create_standards)
    R (do_help)
    R (do_who)
    R (do_time)
    R (do_weather)
    R (do_tackle)
    R (do_circle)
    R (do_bash)
    R (do_kick)
    R (do_tornado)
    R (do_palm)
    R (do_nerve)
    R (do_stance)
    R (do_impale)
   /* R (do_charge) */
    R (do_flash)
    R (do_kill)
    R (do_flurry)
    R (do_flail)
    R (do_warcry)
    R (do_backstab)
    R (do_bs)
    R (do_flee)
    R (do_rescue)
    R (do_guard)
    R (do_unguard)
    R (do_disarm)
    R (do_sla)
    R (do_slay)
    R (do_backward)
    R (do_forward)
    R (do_2backward)
    R (do_2forward)
    R (do_meditate)
    R (do_aedit)
    R (do_redit)
    R (do_oedit)
    R (do_medit)
    R (do_cedit2)
    R (do_cedit)
    R (do_mindex)
    R (do_oindex)
    R (do_resets)
    R (do_astat)
    R (do_asave)
    R (do_busy)
    R (do_alias)
    R (do_qui)
    R (do_quit)
    R (do_delete)
    R (do_allsave)
    R (do_save)
    R (do_follow)
    R (do_ditch)
    R (do_group)
    R (do_split)
    R (do_purgearea)
    R (do_mreset)
    R (do_oreset)
    R (do_ignore)
    R (do_gen_gos)
/*R(do_immtalk)
   R(do_clantalk)
   R(do_egossip)
   R(do_ggossip)
   R(do_ic)
   R(do_godtalk)
 */
    R (do_say2)
    R (do_reply)
    R (do_quiet)
    R (do_emote2)
    R (do_gtell)
    R (do_gtell2)
    R (do_mount)
    R (do_dismount)
    R (do_buck)
    R (do_track)
    R (do_ptrack) /* JRAJRA - player hunting stuff */    
    R (do_convert)
    R (do_donate)  /* JRAJRA - donate command */
    R (do_bank)
    R (do_bankset)
    //R (do_choosesides)       
    //CHOOSE SIDES COMMENTED OUT TO PREVENT ALIGNMENT OF GOOD!! Kilith 05 to stop alignment choosing
    //comment out do_choosesides point portal to room 105 from room 100. 
    R (do_remort)
    R (do_balance)
    R (do_store)
    R (do_unstore)
    R (do_deposit)
    R (do_withdraw)
    R (do_mobscatter)
    R (do_scatter)
    R (do_alien)
    R (do_transfer)
    R (do_afk)
    R (do_players) 
    R (do_pdelete)
    R (do_nodelete)
    R (do_coins)
    R (do_sforce)
    R (do_mplay)
    R (do_answer)
    R (do_investigate)
    R (do_memory)
    R (do_world)
    R (syslag)
    R (finger)
    R (do_beep)
    R (do_fight)
    R (do_wimpy)
    R (do_blank)
    R (do_pagelen)
    R (do_prompt)
    R (do_auto)
    R (do_autoloot)
    R (do_autoscan)
    R (do_autosac)
    R (do_autoexit)
    R (do_autogold)
    R (do_autosplit)
    R (do_assist)
    R (do_ansi)
    R (do_color)
    R (do_config)
    R (do_title)
    R (do_description)

    R (do_armor)
    R (do_weight)
    R (do_affect)
    R (do_eeffect)
    R (do_attribute)
    R (do_info)
    R (do_vscore)
    R (do_score)
    R (do_quickscore)
    R (do_quickscore2)
    R (do_note)
    R (do_drag)
    R (do_get)
    R (do_take)
    R (do_put)
    R (do_drop)
    R (do_give)
    R (do_fill)
    R (do_empty)
    R (do_drink)
    R (do_eat)
    R (do_exchange)
    R (do_swap)
    R (do_wear)
    R (do_wield)
    R (do_hold)
    R (do_sheath)
    R (do_drawtop)
    R (do_drawrooms)
    R (do_draw)
    R (do_remove)
    R (do_sacrifice)
    R (do_bury)
    R (do_quaff)
    R (do_recite)
    R (do_read)
    R (do_brandish)
    R (do_zap)
    R (do_study)
    R (do_light)
    R (do_load)
    R (do_shoot) 
    R (do_throw)
    R (do_look)
    R (do_extinguish)
    R (do_butcher)
    R (do_trophy)
    R (do_topten)
    R (do_pkstat)
    R (mana)
    R (do_clan_2)
    R (do_clan)
    R (do_consider)
    R (do_rating)
    R (do_spray)
    R (do_cast)
    R (do_stand)
    R (do_rest)
    R (do_sleep)
    R (do_wake)
    R (do_inventory)
    R (do_showloot)
    R (do_areas)
    R (do_at)
    R (do_exits)
    R (do_tell)
    R (do_emote)
    R (do_say)

    R (do_north)
    R (do_east)
    R (do_south)
    R (do_west)
    R (do_up)
    R (do_down)

    R (do_disable)
    R (do_capture)
    return;
}
