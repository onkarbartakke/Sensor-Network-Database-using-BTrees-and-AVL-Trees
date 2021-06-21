#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

#include "btree.h"
#include "avlrepo.h"
#include"avl_aqi_repo.h"

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



/*
typedef enum{FAILURE,SUCCESS} status_code;
typedef enum {FALSE,TRUE}BOOL;
*/


BOOL Update_central_Repository(AVL_Node **root1, record record_data) //This is updated n regular basis as soon as any sensor records data
{
    *root1=insertAVL(*root1,record_data);
    if(*root1==NULL)
    return FALSE;
    else
    return TRUE;
}


void Install_new_sensor(sensor_key newinstall, BTree_node **root, int id)
{
    int tpos;
    BTree_node *q;
    q=Search(id,*root,&tpos);
    sensor_key p;

    p=q->key[tpos];
    if(p.sensor_loc==newinstall.sensor_loc && p.sensor_type==newinstall.sensor_type) // sensor locations are checked if same type of sensor
    {                                                                                //present at same station, intallation canceled
        printf("\nThis Type of Sensor already Exits at Station(location)\n");
        return;
    }
    else
    {
        *root=Insert(newinstall, *root);
    }
}

void printdata(AVL_Node *p)
{
    printf("\n");
    printf("\nSensor ID : %d",p->record_data.sensor_ID);
    printf("\nDate :-  %d-%d-%d  Time :- %d:%d ",(p->record_data.d.day),(p->record_data.d.month),(p->record_data.d.year),(p->record_data.t.hour),(p->record_data.t.min));
    printf("\nReading :-  %f",p->record_data.Data);
}

void Traverse_inorder(AVL_Node *p, char sensor_type)
{
    if(p==NULL)
    return;

    Traverse_inorder(p->lchild,sensor_type);

    if(p->record_data.sensor_type==sensor_type)
    {
       printdata(p);
    }

    Traverse_inorder(p->rchild,sensor_type);
}


void Retireve_info_till_Date(AVL_Node *root1, char sensor_type)
{
    if(root1==NULL)
    {
        printf("\nRepository is empty\n");
        return;
    }
    Traverse_inorder(root1,sensor_type);
}

BOOL Date_in_range(Date d, Date d1, Date d2)  // Returns TRUE if d lies between d1 and d2 i.e  (d1<d<d2) Acc to calender
{
    if(d.year<d1.year)
    return FALSE;

    if(d.year==d1.year && d.month<d1.month)
    return FALSE;

    if(d.year==d1.year && d.month==d1.month && d.day<d1.day)
    return FALSE;

    if(d.year>d2.year)
    return FALSE;

    if(d.year==d2.year && d.month>d2.month)
    return FALSE;

    if(d.year==d2.year && d.month==d2.month && d.day>d2.day)
    return FALSE;

    return TRUE;
}

BOOL Date_greater_than_upper_bound(Date d, Date d1)
{
    if(d.year>d1.year)
    return TRUE;

    if(d.year==d1.year && d.month>d1.month)
    return TRUE;

    if(d.year==d1.year && d.month==d1.month && d.day>d1.day)
    return TRUE;

    return FALSE;
}

void Traverse_inorder_date_range(AVL_Node *p, Date d1, Date d2, char sensor_type) 
{
    if(p==NULL)
    return;

    Traverse_inorder_date_range(p->lchild,d1,d2,sensor_type);

    if(Date_in_range(p->record_data.d,d1,d2))
    {
        printdata(p);
    }
    else if(Date_greater_than_upper_bound(p->record_data.d,d2))
    {
        return;
    }

    Traverse_inorder_date_range(p->rchild,d1,d2,sensor_type);
}


void Retirev_info_in_date_range(AVL_Node *root1, Date d1, Date d2, char sensor_type) //Getting info in date Range d1 and d2 from central Repo(AVL)
{
    Traverse_inorder_date_range(root1,d1,d2,sensor_type);
}

/*
Traverses inorder checking consecutive  dates of entry for same  sensor (id specific)
if the consecutive dates are mre than 2 mnths apart sensor is decalred idle and delet from main data base 
*/

void Traverse_dates(int id, AVL_Node *p,Date d1, Date d2, int *f )
{
    if(*f==1)
    return;
                                    
    if(p==NULL)
    return;

    Traverse_dates(id,p,d1,d2,f);

    if(id==p->record_data.sensor_ID)
    {
        d1=d2;
        d2=p->record_data.d;

        if(d1.year==d2.year && abs(d1.month-d2.month)>=2)
        {
            (*f)=1;
            return;
        }

        if(d1.year+1==d2.year && (d1.month!=11) && (d2.month!=1))
        {
            (*f)=1;
            return;
        }
    }

    Traverse_dates(id,p,d1,d2,f);
}


BOOL Idle(int id, AVL_Node *root1)
{
    Date d1,d2;
    BOOL b=FALSE;
    int flag=0;
    Traverse_dates(id,root1,d1,d2,&flag);
    if(flag==1)
    {
        b=TRUE;
    }

    return b;
}


/*
Traversing B tree , checking for each sensor if idle.
Btree is traversed to get id , then for that id  central repo is checked
*/
void Traverse_bt(BTree_node **root, BTree_node *p, AVL_Node *root1) 
{
    if(p==NULL)
    return;

    int i;
    for(i=0;i<p->count;i++)
    {
        Traverse_bt(root,p->branch[i],root1);
        if(Idle(p->key[i].sensor_ID,root1))
        {
            *(root)=Delete(p->key[i].sensor_ID,(*root));
        }
    }  

    Traverse_bt(root,p->branch[i],root1);
}

void Remove_idle_sensors(BTree_node **root,  AVL_Node *root1)
{
    BTree_node *p;
    p=(*root);
    Traverse_bt(root,p,root1);
} 

/*
Btree is traversed inorder till higher limit is crossed
while doing so if id lies in range (low<=id<=high) , sensor info is printed 
*/
void Traverse_in_range(BTree_node *p, int low , int high)
{
    if(p==NULL)
    return;
    int i;

    if(p->key[0].sensor_ID>high)
    return;

    for(i=0;i<p->count;i++)
    {
        Traverse_in_range(p->branch[i],low,high);

        if(p->key[i].sensor_ID>=low && p->key[i].sensor_ID<=high)
        {
            printf("\n\nSensor ID : %d\n",p->key[i].sensor_ID);
            printf("Sensor Location: %c\n",p->key[i].sensor_loc);
            printf("Sensor Type: %c\n",p->key[i].sensor_type);
            printf("Sensor Duration: %d\n\n",p->key[i].duration);
        }
    }
        Traverse_in_range(p->branch[i],low,high);
}


void Find_sensors_in_bewtween(BTree_node *root, int low, int high)
{
    Traverse_in_range(root,low,high);
}


/*
Traverses AQI central Repo for finding  month having max AQI of the year for all stations
so when year is changed, month having max value is printed for last year

when year is same comparison for max value goes on,n month is saved in date
*/
void TraverseinorderAQI(AQI_sensor *p1, AQIRec_node *r1,float value, Date d)
{
    if(r1==NULL)
    return;

    TraverseinorderAQI(p1,r1->lchild,value,d);
    if(p1->sensor_ID==r1->sensor_ID && r1->date.year==d.year && value < r1->AQI_of_day)
    {
        value=r1->AQI_of_day;
        d.month=r1->date.month;
    }
    else if(p1->sensor_ID==r1->sensor_ID && r1->date.year!=d.year)
    {
        printf("\nYear :- %d --> MAx AQI month %d \n",d.year,d.month);
        printf("\nValur : %f \n",value);
        d=r1->date;
        value=r1->AQI_of_day;
    }
     TraverseinorderAQI(p1,r1->rchild,value,d);
} 

void  Find_month_for(AQI_sensor *p1, AQIRec_node *r1)
{
    if(r1==NULL)
    return;
    float value=0.0;
    Date d;                                 
    d=r1->date;
    TraverseinorderAQI(p1,r1,value,d);
}


void Traverse_aqi_sensors(AQI_sensor *p1, AQIRec_node *r1)
{
    if(p1==NULL)
    return;

    Traverse_aqi_sensors(p1->lchild,r1);
    Find_month_for(p1,r1); //Find max aqi month for sesnor p1
    Traverse_aqi_sensors(p1->rchild,r1);
}


void Find_max_AQI_months(AQI_sensor *p1, AQIRec_node *r1)
{
    Traverse_aqi_sensors(p1,r1);  //Traverse Each aqi sensor and then find max aqi month for each year of each sensor
}

/*
While traversing inorder if AQI is > 501 its hazardus output the date
*/
void Traverse_haz(AQIRec_node *r1)
{
    if(r1==NULL)
    return;

    Traverse_haz(r1->lchild);
    if(r1->AQI_of_day >= 501)
    {
        printf("\nHazordous Health Status Recorded on %d - %d - %d \n",r1->date.day,(r1->date.month),(r1->date.year));
    }

    Traverse_haz(r1->rchild);
}

void Display_hazardous_dates(AQIRec_node *r1)
{
    if(r1==NULL)
    {
        printf("\nAQI REPO EMPTY\n");
        return;
    }

    Traverse_haz(r1);
}

int main()
{
    int num_of_sensors,i,duration,ID,j;
    float data;
	printf("Enter number of sensors in the Data Base : ");
	scanf("%d",&num_of_sensors);
	char sensor_type, location;
	Date date;
    Time time;

   BTree_node *root=NULL;
   
   AVL_Node *root1=NULL;

   AQI_sensor *AQIroot=NULL;

   AQIRec_node *AQI_rec_root=NULL;

    printf("\nEnter the details of each of this %d sensors :- \n",num_of_sensors);
    for(i=0;i<num_of_sensors;i++)
    {
        printf("\nEnter sensor ID : ");
        scanf("%d",&ID);

        printf("\nEnter sensor type :\n ");
        scanf("%c",&sensor_type);
      
        printf("\nEnter sensor location :\n ");
        scanf("%c",&location);

        printf("\nEnter the Reading-interval(duration) of a sensor : ");
        scanf("%d",&duration);

        sensor_key newkey;
        newkey.sensor_ID=ID;
        newkey.sensor_type=sensor_type;
        newkey.sensor_loc=location;
        newkey.duration=duration;

        root=Insert(newkey,root);
    }



    int run=1;
    while(run)
    {
        printf("\n\n");
        printf("Select from the following :-  \n");
        printf("\n1) Update Central Repository");
        printf("\n2) Intall new sensor");
        printf("\n3) Retrieve Info");
        printf("\n4) Remove Idle sensors");
        printf("\n5) Find Sensors in Between");
        printf("\n6) To Enter in AQI department");
        printf("\n7) Exit the program\n");

        scanf("%d",&run);
        int id;
        switch(run)
        {
            case 1:
                printf("\nEnter the sensor id : ");
                scanf("%d",&id);
                printf("Enter the sensor type : ");
                scanf("%c",&sensor_type);
                printf("Enter date in DD MM YY format : ");
                scanf("%d %d %d",&date.day,&date.month,&date.year);
                printf("Enter Time is HH MM : ");
                scanf("%d %d",&time.hour,&time.min);
                printf("Enter the Data Reading : ");
                scanf("%f",&data);
               
                record record_data;
                record_data.sensor_ID=id;
                record_data.sensor_type=sensor_type;
                record_data.Data=data;
                record_data.d=date;
                record_data.t=time;
                if(Update_central_Repository(&root1,record_data))
                {
                    printf("\nCentral Repo Updated Successfully");
                }
                else
                {
                    printf("\nCentral Repo Update Failed");
                }
                break;
            case 2:
                printf("\nEnter sensor ID : ");
                scanf("%d",&ID);

                printf("\nEnter sensor type : ");
                scanf("%c",&sensor_type);
               

                printf("\nEnter sensor location : ");
                scanf("%c",&location);

                printf("\nEnter the Reading-interval(duration) of a sensor : ");
                scanf("%d",&duration);
               
                sensor_key newinstall;
                newinstall.sensor_ID=ID;
                newinstall.sensor_type=sensor_type;
                newinstall.sensor_loc=location;
                newinstall.duration=duration;
                printf("Enter the ID of Station where it is to be installed : ");
                int iid;
                scanf("%d",&iid);
                Install_new_sensor(newinstall,&root,iid);
                break;
            case 3:
                if(root1==NULL)
                {
                    printf("\nRepository is Empty");
                    break;
                }

                printf("\nSelect from following :- \n");
                printf("\n1) Retrieve info by sensor type till date");
                printf("\n2) Retireve info on specific time interval for specific sensor type\n");

                int op;
                scanf("%d",&op);
                char sensor_type;
                printf("\nEnter the Sensor type : ");
                scanf("%c",&sensor_type);
                Date date;
                Time time;
                Date d1,d2;
                switch(op)
                {
                    case 1:
                        Retireve_info_till_Date(root1,sensor_type);
                        break;
                    case 2:
                        printf("Enter the date range like 'DD MM YY - DD MM YY ': ");
                        scanf("%d %d %d",&d1.day,&d1.month,&d1.year);
                        char c;
                        scanf("%c",&c);
                        scanf("%d %d %d",&d2.day,&d2.month,&d2.year);
                        Retirev_info_in_date_range(root1,d1,d2,sensor_type);
                    default:
                        printf("\nInvalid option\n");
                        break;
                }
                break;
            case 4:
                Remove_idle_sensors(&root,root1);
                printf("\nIdle sensor if prensent Removed\n");
                break;
                
            case 5:
                printf("Enter the Range in which you want to find : \n");
                printf("Enter the Lower Limit : ");
                int low,high;
                scanf("%d",&low);
                printf("Enter the Higher the Limit : ");
                scanf("%d",&high);
                Find_sensors_in_bewtween(root,low,high);
                printf("\n\n");
                break;
                
            case 6:
                printf("\nSelect from foolowing : \n");
                printf("1)Display Month of Max AQI is recorded for all Stations\n");
                printf("2)To know the date on which Hazadous AQI is recorded\n\n");
                int h;
                scanf("%d",&h);
                if(h==1)
                {
                    Find_max_AQI_months(AQIroot,AQI_rec_root);
                }
                else if(h==2)
                {
                   Display_hazardous_dates(AQI_rec_root);
                }
               break;    
            default:
                printf("\nProgram Exited\n");
                run=0;
                break; 

        }
    }

    return 0;
}
    
