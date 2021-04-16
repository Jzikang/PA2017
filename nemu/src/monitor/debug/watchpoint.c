#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	WP *temp;
	temp=free_;
	free_=free_->next;
	//printf("NO1:%d\n",free_->NO);
	return temp;}

void free_wp(WP *wp){
	wp->next=free_;
	free_=wp;

}
WP* set_watchpoint(char *e){
	bool *p=false;
	WP *temp;
	temp=new_wp();
	//printf("NO2:%d\n",temp->NO);
	temp->next=head;
	head=temp;
	strcpy(head->expr,e);
	head->old_val=expr(e,p);
	head->new_val=head->old_val;
	printf("Set watchpoint #%d\n",head->NO);
        printf("expr      = %s\n",head->expr);
        printf("old value = 0x%08x\n",head->old_val);
	return head;}
bool delete_watchpoint(int NO){
	//printf("%d\n",NO);
        if(!head){
                assert(0);}
        else if(head->NO==NO){
		WP *temp;
		temp=head;
                head=head->next;
		//printf("%d\n",NO);
		free_wp(temp);}
        else{
                WP *temp1,*temp2;
                temp1=head;
                temp2=temp1->next;
                while(temp2){
                        if(temp2->NO==NO){
                                temp1->next=temp2->next;
                        	free_wp(temp2);
				break;}
                        else{
                                temp1=temp2;
                                temp2=temp2->next;}
                }
		if(!temp2){
			printf("Point Not Exist\n");
			return false;}
        }
	printf("Watchpoint %d deleted\n",NO);
	return true;
}

void list_watchpoint(void){
	if(!head){
		printf("Point Not Exist");}
	else{
		WP *temp;
		temp=head;
		while(temp){
			printf("%d   %s     0x%08x\n",temp->NO,temp->expr,temp->old_val);
			temp=temp->next;}
	}
}

		
			

	


