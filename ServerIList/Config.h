

#ifndef CONFIG_H
#define CONFIG_H
#include <string>

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])

#ifndef EN
#define EN_REGISTER 1
#define EN_LOGIN 2
#define EN_LOGOUT 3
#endif EN

#ifndef GR
#define GR_GROUP_LIST 10
#define GR_CREATE_GROUP 11
#define GR_JOIN_GROUP 12
#define GR_REMOVE_USER 13
#define GR_CLOSE_GROUP 14
#define GR_SET_STATUS 15
#define GR_INVITE_USER 16
#endif GR

#ifndef LST
#define LST_ADD_ITEM 20
#define LST_REMOVE_ITEM 21
#define LST_SET_ITEM 22
#endif LST

#ifndef CH
#define CH_SEND 30
#endif CH

#ifndef RE
#define RE_RESTORE 40
#endif

#ifndef PGM_SCC
#define PGM_SCC_LOGIN 100
#define PGM_SCC_REGISTER 101
#define PGM_SCC_ACTE_CREATED 102
#define PGM_SCC_ACTE_JOIN 103
#define PGM_SCC_ACTE_CLOSE 104
#define PGM_SCC_ACTE_EXIT 105
#define PGM_GET_GROUPS_LIST 106
#endif PGM_SCC

#ifndef PGM_CTR
#define PGM_CTR_NEW_USER 110
#define PGM_CTR_REMOVE_USER 111
#define PGM_CTR_STATUS_CHANGED 112
#define PGM_CTR_GROUP_CLOSED 113
#define PGM_CTR_USER_LEAVE 114
#endif PGM_CTR

#ifndef PGM_ERR
#define PGM_ERR_LOGIN 120
#define PGM_ERR_REGISTER_INFO 121
#define PGM_ERR_NAME_TAKEN 122
#define PGM_ERR_GROUP_FULL 123
#define PGM_ERR_GROUP_NOT_FOUND 124
#define PGM_ERR_INFO_TOO_LONG 125
#endif PGM_ERR

#ifndef PGM_MER
#define PGM_MER_MESSAGE 130
#define PGM_MER_ACCESS 131
#endif PGM_MER

#ifndef LST_SCC
#define LST_SCC_ADD_ITEM 200
#define LST_SCC_REMOVE_ITEM 201
#define LST_SCC_SET_ITEM 202
#define LST_SCC_ITEM_SUGESSTED 203
#endif LST_SCC

#ifndef ACT_ERR
#define ACT_ERR_ILLEGAL_STATUS 220
#define ACT_ERR_NO_SUCH_USERNAME 221
#endif ACT_ERR

#ifndef CHA
#define CHA_SCC 300
#define CHA_ERR 310
#endif CHA

#ifndef RE_GET
#define RE_GET_RESTORE 400
#endif RE

#ifndef ERR
#define ERR_GENERAL 500
#endif ERR

static int ALL_MESSAGES_CODES[] = { 1, 2, 3, 10, 11, 12, 13, 14, 15, 16, 20, 21, 22, 23, 24, 30, 40,
101, 102, 103, 104, 105, 110, 111, 112, 113, 114, 120, 121, 122,
123, 124, 125, 130, 131, 200, 201, 202, 203, 220, 221, 300, 301, 400, 500 };

static const int N = ARRAY_SIZE(ALL_MESSAGES_CODES);


/**
Searches the array for the first occurrence of the sequence specified by its code.
if there is match return true, else return false
*/
static bool FIND_MSG_CODE(int code)
{
	int left = 0;
	int right = N - 1;
	int middle;
	while (left <= right) {
		middle = floor((left + right) / 2);
		if (ALL_MESSAGES_CODES[middle] > code)
			right = middle - 1;
		else
		if (ALL_MESSAGES_CODES[middle] < code)
			left = middle + 1;
		else
			return true;
	}
	return false;
}



#endif /*CONFIG_H*/