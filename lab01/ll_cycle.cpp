#include "ll_cycle.h"

bool ll_has_cycle(node *head) {
    /* Ваш код должен быть написан только внутри этой функции */
    	node *rabbit;
	if (head->next != 0 && head->next->next != 0) 
	{
		rabbit = head->next->next;
	}
    	else return false;
	node *turtle = head->next;
	while(rabbit->next->next != 0 && turtle->next != 0)
    	{
		turtle = turtle->next;
	    	rabbit = rabbit->next->next;
	    	if (rabbit == turtle) return true;
    	}
	return false;
}
