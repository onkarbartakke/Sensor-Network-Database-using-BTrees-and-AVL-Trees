#ifndef B_TREE_H
#define B_TREE_H

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

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

typedef enum{FAILURE,SUCCESS} status_code;
typedef enum {FALSE,TRUE}BOOL;


//SENSOR NODE
typedef struct sensor_tag
{
    int sensor_ID;
    char sensor_type;
    char sensor_loc;
    int duration;
    
}sensor_key;


#define MAX 4
#define MIN 2

typedef struct BTree_Node_TAG
{
    int count;
    sensor_key key[MAX+1];
    struct BTree_Node_TAG *branch[MAX+1];

}BTree_node;


BOOL search_node(int target_id, BTree_node *p, int *k);
BOOL PushDown(sensor_key newkey, BTree_node *p, sensor_key *x, BTree_node **xr);
void Pushin(sensor_key x, BTree_node *xr, BTree_node *p, int k);
void Split(sensor_key x, BTree_node *xr, BTree_node *p, int k, sensor_key *y, BTree_node **yr);
BTree_node* Delete(int target_id, BTree_node *root);
BOOL RecDelete(int target_id, BTree_node *p);
void Remove(BTree_node *p, int pos);
void MoveRight(BTree_node  *p, int k);
void MoveLeft(BTree_node  *p, int k);
void Combine(BTree_node *p, int k);
void Restore(BTree_node *p, int k);
void Successor(BTree_node *p, int pos);

/*
 Search: traverse 8-tree looking for target
*/
BTree_node* Search(int target_id, BTree_node *root, int *targetpos)
{
    if(root==NULL)
    return NULL;
    else if(search_node(target_id,root,targetpos))
    return root;
    else
    return Search(target_id,root->branch[*targetpos],targetpos);

}

/*
 SearchNode: searches keys in node p for target; returns location k of target, or
branch on which to continue search
*/
BOOL search_node(int target_id, BTree_node *p, int *k)
{
    if(target_id < p->key[0].sensor_ID)
    {
        *k=0;
        return FALSE;
    }
    else
    {
        *k=p->count;
        while((target_id < p->key[*k].sensor_ID) && *k>1)
        {
            (*k)--;
        }

        if(target_id == p->key[*k].sensor_ID)
        return TRUE;
        else
        return FALSE;
    }
}

/*
Insert: inserts newkey into the B-tree with the given root; requires that newkey is not
already present in the tree
*/
BTree_node* Insert(sensor_key newkey, BTree_node *root)
{
    sensor_key x;
    BTree_node *xr;
    BTree_node *p;

    BOOL pushup;

    pushup=PushDown(newkey,root,&x,&xr);

    if(pushup)
    {
        p=(BTree_node*)malloc(sizeof(BTree_node));
        p->count=1;
        p->key[1]=x;
        p->branch[0]=root;
        p->branch[1]=xr;
        return p;
    }

    return root;
}

/*
 PushDown: recursively move down tree searching for newkey
*/
BOOL PushDown(sensor_key newkey, BTree_node *p, sensor_key *x, BTree_node **xr)
{
    int k;
    if(p==NULL)
    {
        *x=newkey;
        *xr=NULL;
        return TRUE;
    }
    else
    {
        if(search_node(newkey.sensor_ID,p,&k))
        {
            printf("\nThis Sensor already exits in the Network\n");
            return FALSE;
        }
        
        if(PushDown(newkey,p->branch[k],x,xr))
        

        if(p->count < MAX)
        {
            Pushin(*x,*xr,p,k);
            return FALSE;
        }
        else
        {
            Split(*x,*xr,p,k,x,xr);
            return TRUE;
        }

        return FALSE;
    }
}

/*
Pushln: inserts key x and pointer xr into node p at position k; requires that the node
was not previously full
*/
void Pushin(sensor_key x, BTree_node *xr, BTree_node *p, int k)
{
    int i;
    for(i=p->count;i>k;i--)
    {
        p->key[i+1]=p->key[i];
        p->branch[i+1]=p->branch[i];
    }

    p->key[k+1]=x;
    p->branch[k+1]=xr;
    p->count++;
}

/*
 Spilt: spllts node *P with key x and pointer xr at position k into nodes *P and * Yr
with median key y 
*/
void Split(sensor_key x, BTree_node *xr, BTree_node *p, int k, sensor_key *y, BTree_node **yr)
{
    int i;
    int median;

    if(k<=MIN)
    median=MIN;
    else
    median=MIN+1;

    *yr=(BTree_node*)malloc(sizeof(BTree_node));

    for(i=median+1;i<=MAX;i++)
    {
        (*yr)->key[i-median]=p->key[i];
        (*yr)->branch[i-median]=p->branch[i];
    }

    (*yr)->count=MAX-median;
    p->count=median;

    if(k<=MIN)
    {
        Pushin(x,xr,p,k);
    }
    else
    {
        Pushin(x,xr,*yr,k-median);
    }

    *y=p->key[p->count];
    (*yr)->branch[0]=p->branch[p->count];
    (p->count)--;
}

/*
 Delete: deletes the key target from the B-tree with the given root *
*/
BTree_node* Delete(int target_id, BTree_node *root)
{
    BTree_node *p;
    if(!RecDelete(target_id,root))
    {
        printf("\nGiven Sensor Id not exited in Data base\n");
    }
    else if(root->count==0)
    {
        p=root;
        root=root->branch[0];
        free(p);
    }
    
    return root;
}

/*
* RecDelete: look for target to delete
*/
BOOL RecDelete(int target_id, BTree_node *p)
{
    int pos;

    BOOL found;

    if(p==NULL)
    return FALSE;


    if(found=search_node(target_id,p,&pos))
    {
        if(p->branch[pos-1])
        {
            Successor(p,pos);
            if(!(found=RecDelete(p->key[pos].sensor_ID,p->branch[pos])))
            {
                printf("\nSensor Id not exited in Data base\n");
            }
        }
        else
        {
            Remove(p,pos);
        }
    }
    else
    {
        found=RecDelete(target_id,p->branch[pos]);
    }

    if(p->branch[pos]!=NULL)
    {
        if(p->branch[pos]->count<MIN)
        Restore(p,pos);

    }

    return found;
}

/*
 Remove: removes key [pos] and branch [pos] from *P
*/
void Remove(BTree_node *p, int pos)
{
    int i;
    for(i=pos+1;i<=p->count;i++)
    {
        p->key[i-1]=p->key[i];
        p->branch[i-1]=p->branch[i];
    }

    p->count--;
}

/*
Successor: replaces p->key [pos] by its immediate successor under natural order
*/
void Successor(BTree_node *p, int pos)
{
    BTree_node *q;

    for(q=p->branch[pos];q->branch[0]!=NULL;q=q->branch[0]);


    p->key[pos]=q->key[1];
}

/*
 Restore: finds a key and inserts it into p->branch [ k]
 */
void Restore(BTree_node *p, int k)
{
    if(k==0)
    {
        if(p->branch[1]->count > MIN)
        {
            MoveLeft(p,1);
        }
        else
        {
            Combine(p,1);
        }
    }
    else if(k==p->count)
    {
        if(p->branch[k-1]->count > MIN)
        {
            MoveRight(p,k);
        }
        else
        {
            Combine(p,k);
        }
    }
    else if(p->branch[k-1]->count > MIN)
    {
        MoveRight(p,k);
    }
    else if(p->branch[k+1]->count > MIN)
    {
        MoveLeft(p,k+1);
    }
    else
    {
        Combine(p,k);
    }
}

/*
MoveRight: move a key to the right
*/
void MoveRight(BTree_node  *p, int k)
{
    int c;
    BTree_node *t;
    t=p->branch[k];

    for(c=t->count;c>0;c--)
    {
        t->key[c+1]=t->key[c];
        t->branch[c+1]=t->branch[c];
    }

    t->branch[1]=t->branch[0];
    t->count++;

    t=p->branch[k-1];
    p->key[k]=t->key[t->count];
    p->branch[k]->branch[0]=t->branch[t->count];
    t->count--;
}

/*
 MoveLeft: move a key to the left
*/
void MoveLeft(BTree_node *p, int k)
{
    int c;
    BTree_node *t;

    t=p->branch[k-1];
    t->count++;

    t->key[t->count]=p->key[k];
    t->branch[t->count]=p->branch[k]->branch[0];

    t=p->branch[k];
    p->key[k]=t->key[1];
    t->branch[0]=t->branch[1];
    t->count--;

    for(c=1;c<=t->count;c++)
    {
        t->key[c]=t->key[c+1];
        t->branch[c]=t->branch[c+1];
    }
}

/*
 Combine: combine adjacent nodes.
*/
void Combine(BTree_node *p, int k)
{
    int  c;
    BTree_node *q;

    BTree_node *l;

    q=p->branch[k];
    l=p->branch[k-1];
    l->count++;
    l->key[l->count]=p->key[k];

    l->branch[l->count]=q->branch[0];
    for(c=0;c<=q->count;c++)
    {
        l->count++;
        l->key[l->count]=q->key[c];
        l->branch[l->count]=q->branch[c];
    }

    for(c=k;c<p->count;c++)
    {
        p->key[c]=p->key[c+1];
        p->branch[c]=p->branch[c+1];
    }
    p->count--;

    free(q);
}

void Traverse(BTree_node *p)
{
    int i;
    if(p!=NULL)
    {
        for(i=0;i<p->count;i++)
        {
            Traverse(p->branch[i]);
            printf("%d ",p->key->sensor_ID);
        }

        Traverse(p->branch[i]);
    }
}

#endif