#ifndef AVL_AQI_REPO_H
#define AVL_AQI_REPO_H

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

#include "btree.h"
#include "avlrepo.h"

/*
NAME - BARTAKKE ONKAR SUHAS
ENROLL - BT19CSE012

R1 BATCH 

SENSOR NETWORK 

SENSOR DATA BASE  IMPLEMENTED BY BTREE
SENSOR CENTRAL REPO IMPLEMENTED BY AVL TREE
AQI REPOSITORY IMPLEMENTED BY AVL TREE


SENSEOR DATA BASE is a BTREE where the key ,(the factor by which its pos is decided is the id)
higher the id it goes right of the root and lesser the id it goes left

SENSOR CENTRAL REPO and AQI REPO both  are AVL trees , where key is the date, newer the daye time stamp it goes right side of the
root, older the date it goes left
*/

typedef struct AQI_tag
{
    int sensor_ID;
    char sensor_loc;
    int duration;
    struct AQI_tag *lchild;
    struct AQI_tag *rchild;

}AQI_sensor;

typedef struct AQI_record_tag
{
    int sensor_ID;
    Date date;
    Time time;

    float avg_PM10;
    float avg_PM2point5;
    float avg_nitrogen_dioxide;
    float avg_sulphur_dioxide;
    float avg_carbon_monoxide;
    float avg_g_level_ozone;

    int std_time_PM10;
    int std_time_PM2point5;
    int std_time_nitrogen_dioxide;
    int std_time_sulphur_dioxide;
    int std_time_carbon_monoxide;
    int std_time_g_level_ozone;

    float AQI_of_day; //Highest

    struct AQI_record_tag *lchild;
    struct AQI_record_tag *rchild;
    int height;

}AQIRec_node;

int NodeHeightaqi(AQIRec_node *p)
{
    int x,y;
    x=(p && p->lchild)?(p->lchild->height):0;
    y=(p && p->rchild)?(p->rchild->height):0;

    return max(x,y)+1; 
}

int BalanceFactoraqi(AQIRec_node *p)
{
    int x,y;
    x=(p && p->lchild)?(p->lchild->height):0;
    y=(p && p->rchild)?(p->rchild->height):0; 

    return x-y; 
}


AQIRec_node* LL_Rotationaqi(AQIRec_node* p)
{
    AQIRec_node *pl=p->lchild;
    AQIRec_node *plr=pl->rchild;

    pl->rchild=p;
    p->rchild=plr;

    p->height=NodeHeightaqi(p);
    pl->height=NodeHeightaqi(pl);

    return pl;
}


AQIRec_node* RR_Rotationaqi(AQIRec_node* p)
{
    AQIRec_node *pr=p->rchild;
    AQIRec_node *prl=pr->lchild;

    pr->lchild=p;
    p->rchild=prl;

    p->height=NodeHeightaqi(p);
    pr->height=NodeHeightaqi(pr);

    return pr;
}


AQIRec_node* LR_Rotationaqi(AQIRec_node* p)
{
    AQIRec_node *pl=p->lchild;
    AQIRec_node *plr=pl->rchild;

    pl->rchild=plr->lchild;
    p->lchild=plr->rchild;

    plr->lchild=pl;
    plr->rchild=p;

    p->height=NodeHeightaqi(p);
    pl->height=NodeHeightaqi(pl);
    plr->height=NodeHeightaqi(plr);

    return plr;
}


AQIRec_node* RL_Rotationaqi(AQIRec_node *p)
{
    AQIRec_node *pr=p->rchild;
    AQIRec_node *prl=pr->lchild;

    p->rchild=prl->lchild;
    pr->lchild=prl->rchild;

    prl->lchild=p;
    prl->rchild=pr;

    p->height=NodeHeightaqi(p);
    pr->height=NodeHeightaqi(pr);
    prl->height=NodeHeightaqi(prl);

    return prl;
}


AQIRec_node* insertAQIRec(AQIRec_node *root, AQIRec_node p )
{
    if(root==NULL)
    {
        root=(AQIRec_node*)malloc(sizeof(AQIRec_node));
        (*root)=p;
        root->lchild=NULL;
        root->rchild=NULL;

        return root;
    }

    BOOL b=compareDataTimeStamp(root->date,root->time,p.date,p.time);

    if(b==FALSE)
    {
        root->lchild=insertAQIRec(root->lchild,p);
    }
    else if(b==TRUE)
    {
        root->rchild=insertAQIRec(root->rchild,p);
    }

    root->height=NodeHeightaqi(root);

    if(BalanceFactoraqi(root)==2 && BalanceFactoraqi(root->lchild)==1)
    return LL_Rotationaqi(root);
    else if(BalanceFactoraqi(root)==2 && BalanceFactoraqi(root->lchild)==-1)
    return LR_Rotationaqi(root);
    else if(BalanceFactoraqi(root)==-2 && BalanceFactoraqi(root->rchild)==-1)
    return RR_Rotationaqi(root);
    else if(BalanceFactoraqi(root)==-2 && BalanceFactoraqi(root->lchild)==1)
    return RL_Rotationaqi(root);

    return root;
}


#endif