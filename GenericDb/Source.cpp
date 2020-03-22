#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct Node
{
	//void* value;
	//char* type;
	int col_num;
	void* value;
	int version;
	struct Node* nextVer;
	struct Node* next;
};
struct update
{
	int commit;
	int* current_update;
	int size;
	int curPos;
};
struct row
{
	char* pk;
	struct Node* next;
	struct update* update;
	struct row* down;
};
struct table
{
	char* name;
	int noOfColumns;
	char** columnNames;
	char** columnTypes;
	struct row* y;
};
struct tableNodes
{
	struct table** ref;
	int index;
};
void* col_types(char* str,char* column)
{
	if (!strcmp(str, "str"))
	{
		char* col = (char*)malloc(20 * sizeof(char));
		strcpy(col, column);
		return (void*)col;
	}
	else if (!strcmp(str, "int"))
	{
		int* x =(int*)malloc(sizeof(int));
		*x = atoi(column);
		return (void*)x;
	}
}
void CreateRow(struct row** y,char* pk, int noOfColumns, char** columnValues, int* col_no,char** col_type)
{
	struct row* newNode=(struct row*)malloc(sizeof(struct row));
	newNode->pk = (char*)malloc(100 * sizeof(char));
	strcpy(newNode->pk,pk);
	//struct Node** columns = (struct Node**)malloc((noOfColumns)*sizeof(struct Node*));
	struct Node* head = NULL;
	struct Node* temp = NULL;
	for (int i = 0; i <(noOfColumns); i++)
	{
		//struct Node* columns = (struct Node*)malloc((noOfColumns)*sizeof(struct Node));
		struct Node* columns = (struct Node*)malloc(sizeof(struct Node));
		columns->value=col_types(col_type[col_no[i]-1],columnValues[i]);
		columns->version = 1;
		temp = head;
		if (i == 0)
		{
			head = columns;
		}
		else
		{
			while (temp->next != NULL)
				temp = temp->next;
			temp->next = columns;
		}
		columns->col_num = col_no[i];
		columns->next = NULL;
		columns->nextVer= NULL;
	}
	newNode->next = head;
	struct update* up = (struct update*)malloc(sizeof(struct update));
	up->commit = 1;
	up->size = 10;
	up->current_update = (int*)malloc(10 * sizeof(int));
	up->current_update[0] = 1;
	up->curPos = 1;
	newNode->update = up;
	newNode->down = NULL;
	if (*y == NULL)
	{
		*y = newNode;
	}
	else
	{
		struct row* append = *y;
		while (append->down != NULL)
		{
			append = append->down;
		}
		append->down = newNode;
	}
	printf("Row Created Successfully\n");
}
struct row* getCurrentNode(struct row* y,char* row_id)
{
	struct row* append = y;
	while (append != NULL)
	{
		if (!strcmp(append->pk,row_id))
			return append;
		append = append->down;
	}
	return NULL;
}
int updateRow(struct row* y, char* row_num, int noOfColumnsUpdate, int* columnNo, char** columnNames, char** col_type)
{
	struct row* m = getCurrentNode(y,row_num);
	if (m == NULL)
	{
		//printf("Enter Valid row Number");
		return -1;
	}
	m->update->current_update[(m->update->curPos)++] = m->update->current_update[m->update->curPos - 1] + 1;
	int version_no = m->update->current_update[(m->update->curPos) - 1];
	struct Node* columns = m->next;
	struct Node** updatecol = (struct Node**)malloc(noOfColumnsUpdate*sizeof(struct Node*));
	for (int i = 0; i < noOfColumnsUpdate; i++)
	{
		updatecol[i] = (struct Node*)malloc(sizeof(struct Node));
		updatecol[i]->col_num = columnNo[i];
		updatecol[i]->value = col_types(col_type[columnNo[i]-1], columnNames[i]);
		updatecol[i]->next = NULL;
		updatecol[i]->nextVer = NULL;
		updatecol[i]->version = -1;
	}
	(m->update->commit)++;
		for (int i = 0; i < noOfColumnsUpdate; i++)
		{
			struct Node* temp = columns;
			while (1)
			{
				if (columns == NULL)
				{
					temp->next = updatecol[i];
				//	updatecol[i]->version = version_no;
					updatecol[i]->version = 1;
					updatecol[i]->nextVer = NULL;
					break;
				}
				else if (columns->col_num == columnNo[i])
				{
					struct Node* n = columns;
					struct Node* nextVersion = columns->nextVer;
					while (nextVersion != NULL)
					{
						n = nextVersion;
						nextVersion = n->nextVer;
					}
					int versionno = n->version;
					n->nextVer = updatecol[i];
					//updatecol[i]->version = version_no;
					updatecol[i]->version = versionno+1;
					updatecol[i]->nextVer = NULL;
					break;
				}
				else if (columns->col_num < columnNo[i])
				{
					temp = columns;
					columns = columns->next;
				}
				else if (columns == temp && columns->col_num > columnNo[i])
				{
					updatecol[i]->next = temp;
					//updatecol[i]->version = version_no;
					updatecol[i]->version = 1;
					updatecol[i]->nextVer = NULL;
					m->next = updatecol[i];
					break;
				}
				else
				{
					struct Node* thirdNode = temp->next;
					updatecol[i]->next = thirdNode;
					//updatecol[i]->version = version_no;
					 updatecol[i]->version = 1;
					updatecol[i]->nextVer = NULL;
					temp->next = updatecol[i];
					break;
				}
			}
		}
		return 1;
}
void getRow(struct row* y,char** column_names,char* row_no, int noOfColumns,char** column_types)
{
	struct row* m = getCurrentNode(y,row_no);
	if (m == NULL)
	{
		printf("Enter Valid row Number\n");
		return;
	}
	struct Node* n = m->next;
	while (n!=NULL)
	{
		struct Node* latestVersion = n;
		struct Node* nextVersion = n->nextVer;
		while (nextVersion != NULL)
		{
			latestVersion = nextVersion;
			nextVersion = latestVersion->nextVer;
		}
		printf("%s ", column_names[latestVersion->col_num - 1]);
		if (!strcmp(column_types[latestVersion->col_num - 1], "int"))
			printf("%d ", *((int*)latestVersion->value));
		else if (!strcmp(column_types[latestVersion->col_num - 1], "str"))
			printf("%s ", (char*)latestVersion->value);
		printf("%d\n",latestVersion->version);
		n = n->next;
	}
}
struct row* getPreviousNode(struct row* y,char* row_no)
{
	struct row* append = y;
	while (append!= NULL)
	{
		if (append->down != NULL)
		{
			if (!strcmp(append->down->pk ,row_no))
				return append;
		}
		append = append->down;
	}
	return NULL;
}
void deleteRow(struct row** y,char** column_names,char* row_no)
{
	struct row* m=*y;
	if (!strcmp(m->pk,row_no))
	{
		*y = (*y)->down;
		return;
	}
	else
	{
		m = getPreviousNode(*y,row_no);
	}
	//struct row* y = m->down->down;
	//m->down = y;
	struct row* y1 = m->down->down;
	m->down = y1;
}
void createTableNodes(struct tableNodes** tables)
{
	struct table* temp = (struct table*)malloc(sizeof(struct table));
	printf("Enter details of table\n");
	printf("Enter table Name\n");
	temp->name = (char*)malloc(100 * sizeof(char));
	scanf("%s", temp->name);
	printf("Enter no. of columns\n");
	scanf("%d",&temp->noOfColumns);
	printf("Enter column names\n");
	temp->columnNames = (char**)malloc(temp->noOfColumns*sizeof(char*));
	for (int i = 0; i < temp->noOfColumns; i++)
	{
		temp->columnNames[i] = (char*)malloc(50 * sizeof(char));
		scanf("%s", temp->columnNames[i]);
	}
	printf("Enter column Types\n");
	temp->columnTypes = (char**)malloc(temp->noOfColumns*sizeof(char*));
	for (int i = 0; i < temp->noOfColumns; i++)
	{
		temp->columnTypes[i] = (char*)malloc(50 * sizeof(char));
		scanf("%s", temp->columnTypes[i]);
	}
	temp->y = NULL;
	(*tables)->ref[(*tables)->index] = temp;
	(*tables)->index++;
}
struct tableNodes* createTables()
{
	struct tableNodes* tables = (struct tableNodes*)malloc(sizeof(struct tableNodes));
	tables->index = 0;
	tables->ref = (struct table**)malloc(10 * sizeof(table*));
	//Users Table-id,name,mobile,email
	createTableNodes(&tables);
	//Followers Table-postid,userid,location
	//createTableNodes(&tables);
	//Posts table-userId,Followers(comma separated values) 
	//createTableNodes(&tables);
	return tables;
}
void load(char* filename,struct row* y,char** colNames,char** colTypes,int noOfColumns)
{
	for (int i = 0; i < noOfColumns; i++)
	{
		struct row* rows = y;
		struct Node* columns = y->next;
		FILE* fp;
		int flag = 0;
		char file[80];
		int index = 0;
		int j = 0;
		while (filename[j] != '\0')
		{
			file[index++] = filename[j];
			j++;
		}
		j = 0;
		while (colNames[i][j] != '\0')
		{
			file[index++] = colNames[i][j];
			j++;
		}
		file[index++] = '.';
		file[index++] = 't';
		file[index++] = 'x';
		file[index++] = 't';

		file[index] = '\0';
		fp = fopen(file, "w");
		if (!strcmp(colTypes[i], "int"))
			flag = 1;
		else if (!strcmp(colTypes[i], "str"))
			flag = 2;
		while (rows != NULL)
		{
			columns = rows->next;
			while (columns != NULL)
			{
				if (columns->col_num == i+1)
				{
					struct Node* latestVersion = columns;
					while (latestVersion != NULL)
					{
						if (flag == 1)
						{
							fprintf(fp, "%s %d %d %d\n",rows->pk,i+1,*((int*)latestVersion->value),latestVersion->version);
						}
						else if (flag == 2)
						{
							fprintf(fp, "%s %d %s %d\n", rows->pk,i+1,(char*)latestVersion->value, latestVersion->version);
						}
						latestVersion = latestVersion->nextVer;
					}
					break;
				}
				else if (columns->col_num>i)
				{
					break;
				}
				else
				{
					columns = columns->next;
				}
			}
			rows = rows->down;
		}
		fclose(fp);
	}
}
char* getline(FILE* fp)
{
	char* res = (char*)malloc(1024 * sizeof(char));
	int index = 0;
	char c = fgetc(fp);
	while (c != '\n' && c != EOF)
	{
		res[index++] = c;
		c = fgetc(fp);
	}
	if (index != 0)
	{
		res[index] = '\0';
		return res;
	}
	return NULL;
}
void inMemory(char* fname,char* tableName,struct row** table,char** columnTypes)
{
	int index = 0;
	while (1)
	{
		if (tableName[index] == fname[index])
		{
			index++;
		}
		else
			break;
	}
	char columnName[30];
	int indexOfColumnName = 0;
	while (fname[index] != '.')
	{
		columnName[indexOfColumnName++] = fname[index];
		index++;
	}
	columnName[indexOfColumnName] = '\0';
	FILE* fp = fopen(fname, "r");
	while (1)
	{
		char* buf = (char*)malloc(100 * sizeof(char));
		buf=getline(fp);
		if (buf == NULL)
			break;
		char* pk=strtok(buf, " ");
		char* cid = strtok(NULL, " ");
		char* value = strtok(NULL, " ");
		char* version = strtok(NULL, " ");
		char** values = (char**)malloc(sizeof(char*));
		values[0] = (char*)malloc(30 * sizeof(char));
		strcpy(values[0],value);
		int* cids = (int*)malloc(sizeof(int));
		cids[0] = atoi(cid);
		int t=updateRow(*table, pk, 1, cids, values,columnTypes);
		if (t == -1)
		{
			CreateRow(table, pk, 1, &value, cids, columnTypes);
		}
	}
}
int main()
{ 
	int no_of_columns;
	int n;
	int i;
	struct tableNodes* tables = createTables();
	char** columnValues;
	char row_num[15];
	int noOfColumnsUpdate;
	int* columnNo;
	char tableName[100];
	int tableIndex;
	while (1)
	{
		printf("Enter 1.To insert a row(PUT). 2.To update a row 3.get row 4.delete row 5.load data into file 6.Deserializing\n");
		scanf("%d", &n);
		printf("Enter table name(User,Post,Follower)");
		scanf("%s", tableName);
			for (int i = 0; i < tables->index; i++)
			{
				if (!strcmp(tables->ref[i]->name, tableName))
				{
					no_of_columns = tables->ref[i]->noOfColumns;
					tableIndex = i;
					break;
				}
			}
		switch (n)
		{
		case 1:
			printf("Enter number of Columns to Update\n");
			scanf("%d", &noOfColumnsUpdate);
			columnNo = (int*)malloc(noOfColumnsUpdate*sizeof(int));
			columnValues = (char**)malloc(noOfColumnsUpdate*sizeof(char*));
			printf("Enter the column Numbers to be updated starting from 1\n");
			for (int i = 0; i < noOfColumnsUpdate; i++)
			{
				scanf("%d", &columnNo[i]);
			}
			printf("Enter the column Values to be updated");
			for (int i = 0; i < noOfColumnsUpdate; i++)
			{
				columnValues[i] = (char*)malloc(30*sizeof(char));
				scanf("%s", columnValues[i]);
			}
			CreateRow(&(tables->ref[tableIndex]->y),columnValues[0],noOfColumnsUpdate, columnValues, columnNo, tables->ref[tableIndex]->columnTypes);
			break;
		case 2:
			printf("Enter Row Number to Update\n");
			scanf("%s", row_num);
			printf("Enter number of Columns to Update\n");
			scanf("%d", &noOfColumnsUpdate);
			columnNo = (int*)malloc(noOfColumnsUpdate*sizeof(int));
			columnValues = (char**)malloc(noOfColumnsUpdate*sizeof(char*));
			printf("Enter the column Numbers to be updated starting from 1\n");
			for (int i = 0; i < noOfColumnsUpdate; i++)
			{
				scanf("%d", &columnNo[i]);
			}
			scanf("Enter the column Values to be updated(Integers)");
			for (int i = 0; i < noOfColumnsUpdate; i++)
			{
				columnValues[i] = (char*)malloc(30 * sizeof(char));
				scanf("%s",columnValues[i]);
			}
			i=updateRow((tables->ref[tableIndex]->y),row_num, noOfColumnsUpdate, columnNo, columnValues,tables->ref[tableIndex]->columnTypes);
			if (i == -1)
				printf("Enter valid column number\n");
			break;
		case 3:
			printf("Enter Row Id\n");
			scanf("%s", row_num);
			getRow(tables->ref[tableIndex]->y, tables->ref[tableIndex]->columnNames, row_num, no_of_columns,tables->ref[tableIndex]->columnTypes);
			break;
		case 4:
			printf("Enter Row Id\n");
			scanf("%s", row_num);
			deleteRow(&(tables->ref[tableIndex]->y),tables->ref[tableIndex]->columnNames, row_num);
			break;
		case 5:
			load(tables->ref[tableIndex]->name,tables->ref[tableIndex]->y, tables->ref[tableIndex]->columnNames, tables->ref[tableIndex]->columnTypes,tables->ref[tableIndex]->noOfColumns);
			break;
		case 6:
			while (1)
			{
				printf("Enter file names");
				char fname[80];
				scanf("%s", fname);
				if (!strcmp(fname, "exit"))
					break;
				inMemory(fname, tableName, &(tables->ref[tableIndex]->y),tables->ref[tableIndex]->columnTypes);
			}
			break;
		}
	}
	return 0;
}