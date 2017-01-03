//type value: CONFIG 0,NORMAL 1,SKILL 2,BORNBUFF 3,BUFF 4,BULLET 5
{
    "LvBu":[{
		"file":"LvBu_Q",
		"skillID":0,
        "name":"LvBuQSkill",
		"cname":"ʮ������",
        "recipes":[{
			"type":2,//skill
            "template":"RunSkillTemplate",
            "name":"LvBuQSkill",
			"cname":"ʮ������"
        },
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuQBuff_speed",
			"cname":"ʮ�����𣨹��٣�"
		},
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuQBuff_passive",
			"cname":"ʮ�����𣨸��ˣ�"
		}
		]
    },
	{
		"file":"LvBu_W",
		"skillID":1,
        "name":"LvBuWSkill",
		"cname":"��ɨǧ��",
        "recipes":[{
			"type":2,//skill
            "template":"SkillTemplate",
            "name":"LvBuWSkill",
			"cname":"��ɨǧ��"
        },
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuWBuff_shield",
			"cname":"��ɨǧ�������ܣ�"
		}
		]
    },
	{
		"file":"LvBu_E",
		"skillID":2,
        "name":"LvBuESkill",
		"cname":"һ�ﵱǧ",
        "recipes":[{
			"type":2,//skill
            "template":"SkillTemplate",
            "name":"LvBuESkill",
			"cname":"һ�ﵱǧ"
        },
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuEBuff_passive",
			"cname":"һ�ﵱǧ"
		},
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuEBuff_speed",
			"cname":"һ�ﵱǧ�������½���"
		}
		]
	},
	{
		"file":"LvBu_R",
		"skillID":3,
        "name":"LvBuRSkill",
		"cname":"ħ����˫",
        "recipes":[{
			"type":2,//skill
            "template":"SkillTemplate",
            "name":"LvBuRSkill",
			"cname":"ħ����˫"
        },
		{
			"type":4,//buff
			"template":"BuffTemplate",
            "name":"LvBuRBuff_stun",
			"cname":"ħ����˫"
		}
		]
	},
	{
		"file":"LvBu_N",
		"skillID":-1,
        "name":"LvBuNSkill",
		"cname":"",
        "recipes":[{
			"type": 2,//skill
            "template":"NormalTemplate",
            "name":"LvBuNSkill",
			"cname":""
        }
		]
	},
	{
		"file":"LvBu_B",
		"skillID":98,
        "name":"LvBuBornBuff",
		"cname":"���컭�",
        "recipes":[{
			"type": 3,//born buff
            "template":"BuffTemplate",
            "name":"LvBuBornBuff",
			"cname":"���컭�"
        },
		{
			"type": 4,// buff
            "template":"BuffTemplate",
            "name":"LvBuBornBuff_armor",
			"cname":"���컭ꪣ����ͻ��ף�"
        }
		]
	},
	{
		"file":"LvBu_config",
		//"skillID":,
        "name":"LvBu_config",
		"cname":"",
        "recipes":[{
			"type": 0,//config
            "template":"ConfigTemplate",
            "name":"LvBu_config",
			"cname":""
        }
		]
	}]
}