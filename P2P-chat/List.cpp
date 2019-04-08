#include "List.h"

void ListAdd(PPList Head, PPList NewEl)
{
	(*Head)->Last->Next = (*NewEl);
	(*Head)->Last = (*NewEl);
}

void ListDelete(PPList Head, PPList ForDelEl)
{
	PList Tmp = (*Head);

	while (Tmp->Next != (*ForDelEl))
	{
		Tmp = Tmp->Next;
	}
	Tmp->Next = (*ForDelEl)->Next;

	if ((*Head)->Last == (*ForDelEl))
	{
		free((*ForDelEl));
		(*Head)->Last = Tmp;
	}
	else
	{
		free((*ForDelEl));
	}
}