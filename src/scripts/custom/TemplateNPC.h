#ifndef TALENT_FUNCTIONS_H
#define TALENT_FUNCTIONS_H

struct TalentTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint32         talentId;
};

struct template_npc_talents
{
    uint32          temp_id;
    std::string     class_name;
    uint32          talent_id;
    uint32          rank;
};

struct template_npc_gear
{
    uint32          temp_id;
    std::string     class_name;
    std::string     gossip_text;
    uint32          item_slot;
    uint32          item_entry;
    uint32          item_enchant;
    uint32          talent_tab_id;
    uint8           patch;
};

typedef std::vector<template_npc_talents*> TemplateNpcTalentsMap;
typedef std::vector<template_npc_gear*> TemplateNpcGearMap;

class sTemplateNPC
{
public:
    static sTemplateNPC* instance()
    {
        static sTemplateNPC* instance = new sTemplateNPC();
        return instance;
    }
    void LoadTalentsContainer();
    std::string sTalentsSpec;

    TemplateNpcTalentsMap m_TemplateNpcTalentsMap;
    TemplateNpcGearMap m_TemplateNpcGearMap;
};
#define sTemplateNpcMgr sTemplateNPC::instance()
#endif
