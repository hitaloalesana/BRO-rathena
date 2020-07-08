// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom battle config initializations
 * Place the definition of custom battle configuration initializations in this file.
 *
 * Do not forget to add a comma at the end of the line, if you want to add another config
 *
 * Format:
 *	 { "name", &battle_config.<variable name>, <default value>, <minimum value>, <maximum value> },
 **/

	{ "bg_reward_rates",                    &battle_config.bg_reward_rates,                 100,    0,      INT_MAX,        },
	{ "bg_reportafk_leaderonly",            &battle_config.bg_reportafk_leaderonly,         1,      0,      1,              },
	{ "bg_idle_announce",                   &battle_config.bg_idle_announce,                0,      0,      INT_MAX,        },
	{ "bg_idle_autokick",                   &battle_config.bg_idle_autokick,                0,      0,      INT_MAX,        },
	{ "bg_queue_onlytowns",                 &battle_config.bg_queue_onlytowns,              1,      0,      1,              },
	{ "bg_logincount_check",                &battle_config.bg_logincount_check,             1,      0,      1,              },
	{ "bg_queue2team_balanced",             &battle_config.bg_queue2team_balanced,          1,      0,      1,              },
	{ "bg_reserved_char_id",                &battle_config.bg_reserved_char_id,             999996, 0,      INT_MAX,        },
	{ "woe_reserved_char_id",               &battle_config.woe_reserved_char_id,            999999, 0,      INT_MAX,        },
	{ "bg_invincible_time",                 &battle_config.bg_invincible_time,              5000,   0,      INT_MAX,        },
