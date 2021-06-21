#ifndef AVL_REPO_H
#define AVL_REPO_H

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

#include "btree.h"
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

//DATE
typedef struct date_tag
{
	int day;
	int month;
	int year;
}Date;

//TIME
typedef struct time_tag
{
	int hour;
	int min;
}Time;


typedef struct record_tag
{
    int sensor_ID;
    char sensor_type;
    struct date_tag d;
    struct time_tag t;
    float Data;

}record;

//This goes in AVL tree as a node
typedef struct AVL_Node_tag
{
    record record_data;
    struct AVL_Node_tag *lchild;
    struct AVL_Node_tag *rchild;
    int height;//Height Of that node

}AVL_Node;

int max(int a, int b)
{
    if(a>b)
    return a;
    else
    return b;
}

//Height Of that node
int NodeHeight(AVL_Node *p)
{
    int x,y;
    x=(p && p->lchild)?(p->lchild->height):0;
    y=(p && p->rchild)?(p->rchild->height):0;

    return max(x,y)+1;
}


int BalanceFactor(AVL_Node *p)
{
    int x,y;
    x=(p && p->lchild)?(p->lchild->height):0;
    y=(p && p->rchild)?(p->rchild->height):0; 

    return x-y;  
}


AVL_Node* LL_Rotation(AVL_Node* p)
{
    AVL_Node *pl=p->lchild;
    AVL_Node *plr=pl->rchild;

    pl->rchild=p;
    p->rchild=plr;

    p->height=NodeHeight(p);
    pl->height=NodeHeight(pl);

    return pl;
}


AVL_Node* RR_Rotation(AVL_Node* p)
{
    AVL_Node *pr=p->rchild;
    AVL_Node *prl=pr->lchild;

    pr->lchild=p;
    p->rchild=prl;

    p->height=NodeHeight(p);
    pr->height=NodeHeight(pr);

    return pr;
}


AVL_Node* LR_Rotation(AVL_Node* p)
{
    AVL_Node *pl=p->lchild;
    AVL_Node *plr=pl->rchild;

    pl->rchild=plr->lchild;
    p->lchild=plr->rchild;

    plr->lchild=pl;
    plr->rchild=p;

    p->height=NodeHeight(p);
    pl->height=NodeHeight(pl);
    plr->height=NodeHeight(plr);

    return plr;
}


AVL_Node* RL_Rotation(AVL_Node *p)
{
    AVL_Node *pr=p->rchild;
    AVL_Node *prl=pr->lchild;

    p->rchild=prl->lchild;
    pr->lchild=prl->rchild;

    prl->lchild=p;
    prl->rchild=pr;

    p->height=NodeHeight(p);
    pr->height=NodeHeight(pr);
    prl->height=NodeHeight(prl);

    return prl;
}

/*
If date time stamp d1,t1 is older than date time stamp d2,t2 it returns True else false
*/
BOOL compareDataTimeStamp(Date d1, Time t1, Date d2, Time t2)
{
    if(d1.year < d2.year)
    return TRUE;
    else if(d1.year > d2.year)
    return FALSE;

    if(d1.month < d2.month)
    return TRUE;
    else if(d1.month > d2.month)
    return FALSE;

    if(d1.day < d2.day)
    return TRUE;
    else if(d1.day > d2.day)
    return FALSE;

    if(t1.hour < t2.hour)
    return TRUE;
    else if(t1.hour > t2.hour)
    return FALSE;

    if(t1.min < t2.min)
    return TRUE;
    else if(t1.min > t2.min)
    return FALSE;

    return TRUE;
    /*
    This is because if date time matches exactly , that means sensor type differs, and in 
    such case it must always inserted on right side
    */
}

AVL_Node* insertAVL(AVL_Node* p, record data)
{
    if(p==NULL)
    {
        AVL_Node *t;
        //t=new Node(d);
        t=(AVL_Node*)malloc(sizeof(AVL_Node));
        t->record_data=data;
        t->height=1;
        return t;
    }

    BOOL b=compareDataTimeStamp(p->record_data.d, p->record_data.t, data.d, data.t);
    if(b==TRUE)
    {
        p->rchild=insertAVL(p->rchild,data);
    }
    else if(b==FALSE)
    {
        p->lchild=insertAVL(p->lchild,data);
    }

    p->height=NodeHeight(p);

    if(BalanceFactor(p)==2 && BalanceFactor(p->lchild)==1)
    return LL_Rotation(p);
    else if(BalanceFactor(p)==2 && BalanceFactor(p->lchild)==-1)
    return LR_Rotation(p);
    else if(BalanceFactor(p)==-2 && BalanceFactor(p->rchild)==-1)
    return RR_Rotation(p);
    else if(BalanceFactor(p)==-2 && BalanceFactor(p->lchild)==1)
    return RL_Rotation(p);

    return p;
}


int Height(AVL_Node *p)
{
    if(p==NULL)
    return 0;

    int x=Height(p->rchild);
    int y=Height(p->lchild);
    return max(x,y)+1;
}

/* 
Returns Inorder predecssor
*/
AVL_Node* InPred(AVL_Node *p)
{
    while(p && p->rchild)
    {
        p=p->rchild;
    }

    return p;
}

/*
Returns Inorder Successor
*/
AVL_Node* InSucc(AVL_Node *p)
{
    while(p && p->lchild)
    {
        p=p->lchild;
    }

    return p;
}

//True if date time stamp is Equal
BOOL Equals(Date d1, Time t1, Date d2, Time t2) 
{
    if(d1.day==d2.day && d1.month==d2.month && d1.year==d2.year)
    {
        if(t1.hour==t2.hour && t1.min==t2.min)
        {
            return TRUE;
        }
    }

    return FALSE;
}

AVL_Node* DeleteAVL(AVL_Node *p, record key)
{
    if(p==NULL)
    return NULL;
    //Alqays a leaf node Deleted
    if(Equals(p->record_data.d, p->record_data.t, key.d, key.t) && p->lchild==NULL && p->rchild==NULL)
    {
        free(p);
        return NULL;
    }

    BOOL b=compareDataTimeStamp(p->record_data.d, p->record_data.t, key.d, key.t);
    if(Equals(p->record_data.d, p->record_data.t, key.d, key.t)) //if equals
    {
        AVL_Node *q; //if height of left subtree is high, bring inorder predessor in that place
        if(Height(p->lchild) > Height(p->rchild))
        {
           
            q=InPred(p->lchild);
            p->record_data=q->record_data;
            p->lchild=DeleteAVL(p->lchild,q->record_data);
        }
        else
        {    //if height of right subtree is high, bring inorder succesor in that place
            q=InSucc(p->rchild);
            p->record_data=q->record_data;
            p->rchild=DeleteAVL(p->rchild,q->record_data);
        }
    }
    else if(b==TRUE)
    {
       p->rchild=DeleteAVL(p->rchild,key);
    }
    else if(b==FALSE)
    {
        p->lchild=DeleteAVL(p->lchild,key);
    }

    p->height=NodeHeight(p);


    if(BalanceFactor(p)==2 && BalanceFactor(p->lchild)==1)
    return LL_Rotation(p);
    else if(BalanceFactor(p)==2 && BalanceFactor(p->lchild)==-1)
    return LR_Rotation(p);
    else if(BalanceFactor(p)==-2 && BalanceFactor(p->rchild)==-1)
    return RR_Rotation(p);
    else if(BalanceFactor(p)==-2 && BalanceFactor(p->lchild)==1)
    return RL_Rotation(p);
    else if(BalanceFactor(p)==2 && BalanceFactor(p->lchild)==0)
    return LL_Rotation(p);
    else if(BalanceFactor(p)==-2 && BalanceFactor(p->rchild)==0)
    return RR_Rotation(p);

    return p;
}

#endif
