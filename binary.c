#include <stdio.h>
#include <string.h>
#include <math.h>
#define LEN 26
short int trie[(1 << 15)][26], cnt[(1 << 15)], deleteStack[1000], availableStack[(1 << 15)], deleteTop;
unsigned short int end, availableTop;
FILE *fp, *fpStack,*fpCnt;

short int nextAvailableNode()
{
    //if stack is empty
    if (availableTop == 0)
        return ++end;
    if (availableStack[availableTop - 1] == 1)
        return availableStack[availableTop -= 2];
    return availableStack[--availableTop];
}

void insertTrie(char *str)
{
    int curr = 1, i;
    short int child;
    for (i = 0; str[i + 1]; i++)
    // if (trie[curr][str[i] - '0'])
    //     curr = (trie[curr][str[i] - '0'] < 0) * -(trie[curr][str[i] - '0']) + (trie[curr][str[i] - '0'] >= 0) * trie[curr][str[i] - '0'];
    // else
    // {
    //     cnt[curr]++;
    //     curr = trie[curr][str[i] - '0'] = nextAvailableNode();
    //     printf("%c %d\n", str[i], curr);
    // }
    {
        fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
        //printf("PosRead=%ld ",ftell(fp));
        fread(&child, sizeof(short int), 1, fp);
        //printf("%d: child=%d ",i,child);
        if (child)
            curr = (child < 0) * -child + (child >= 0) * child;
        else
        {
            cnt[curr]++;
            fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);     //Important step. seek. allocate. write.
            curr = nextAvailableNode();
            fwrite(&curr, sizeof(short int), 1, fp);
            //printf("str[i]=%c curr=%d pos=%ld\n", str[i], curr,ftell(fp));
            printf("%c %d\n",str[i],curr);
        }
    }
    //printf(" last=%d ",child);
    // if (trie[curr][str[i] - '0'] < 0)
    //     printf("%s already present\n", str);
    // else if (trie[curr][str[i] - '0'] > 0)
    // {
    //     trie[curr][str[i] - '0'] *= -1;
    //     printf("%s is inserted\n", str);
    // }
    // else
    // {
    //     cnt[curr]++;
    //     trie[curr][str[i] - '0'] = -nextAvailableNode();
    //     printf("%c %d\n", str[i], trie[curr][str[i] - '0']);
    //     printf("%s is inserted\n", str);
    // }
    fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
    fread(&child, sizeof(short int), 1, fp);
    if (child < 0)
        printf("%s already present\n", str);
    else if (child > 0)
    {
        child *= -1;
        fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
        fwrite(&child, sizeof(short int), 1, fp);
        printf("%s is inserted\n", str);
    }
    else
    {
        cnt[curr]++;
        child = -nextAvailableNode();
        fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
        fwrite(&child, sizeof(short int), 1, fp);
        printf("%c %d\n", str[i], child);
        printf("%s is inserted\n", str);
    }
}

void searchTrie(char *str)
{
    int curr = 1, i;
    short int child;
    for (i = 0; str[i + 1]; i++)
    // if (trie[curr][str[i] - '0'])
    //     curr = (trie[curr][str[i] - '0'] < 0) * -(trie[curr][str[i] - '0']) + (trie[curr][str[i] - '0'] >= 0) * trie[curr][str[i] - '0'];
    // else
    // {
    //     printf("%s not present\n", str);
    //     return;
    // }
    {
        fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
        fread(&child, sizeof(short int), 1, fp);
        if (child)
            curr = (child < 0) * -child + (child >= 0) * child;
        else
        {
            printf("%s not present\n", str);
            return;
        }
    }
    fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
    fread(&child, sizeof(short int), 1, fp);
    if (child < 0)
        printf("%s is present\n", str);
    else
        printf("%s not present\n", str);
}

void deleteTrie(char *str)
{
    int curr = 1, i;
    short int child;
    deleteTop = 0;
    for (i = 0; str[i + 1]; i++)
    {

        deleteStack[deleteTop++] = curr;

        fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
        fread(&child, sizeof(short int), 1, fp);
        if (child)
            curr = (child < 0) * -child + (child >= 0) * child;
        else
        {
            printf("%s not present\n", str);
            return;
        }
    }
    // if (trie[curr][str[i] - '0'] < 0)
    // {
    //     if (cnt[trie[curr][str[i] - '0']] > 0) //Remove only flag and retain node, for the sake of its other children
    //         trie[curr][str[i] - '0'] *= -1;
    //     else
    //     {
    //         availableStack[availableTop++] = -trie[curr][str[i] - '0'];
    //         while (deleteTop >= 0)
    //         {
    //             trie[curr][str[i] - '0'] = 0;
    //             i--;
    //             cnt[curr]--;
    //             if (cnt[curr] > 0)
    //                 break;
    //             availableStack[availableTop++] = curr;
    //             deleteTop--;
    //             if (deleteTop >= 0)
    //                 curr = deleteStack[deleteTop];
    //         }
    //     }
    //     printf("%s deleted\n", str);
    // }
    fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
    fread(&child, sizeof(short int), 1, fp);
    if (child < 0)
    {
        if (cnt[child] > 0)
        {
            child *= -1;
            fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
            fwrite(&child, sizeof(short int), 1, fp);
        }
        else
        {
            availableStack[availableTop++] = -child;
            while (deleteTop >= 0)
            {
                child = 0;
                fseek(fp, 2 * (curr * LEN + (str[i] - '0')), SEEK_SET);
                fwrite(&child, sizeof(short int), 1, fp);
                i--;
                cnt[curr]--;
                if (cnt[curr] > 0)
                    break;
                availableStack[availableTop++] = curr;
                deleteTop--;
                if (deleteTop >= 0)
                    curr = deleteStack[deleteTop];
            }
        }
        printf("%s deleted\n", str);
    }
    else
        printf("%s not present\n", str);
}
void printStack()
{
    for (int i = 0; i < availableTop; i++)
        printf("%d ", availableStack[i]);
    printf("---------> Available Stack\n");
}
int main()
{
    fp = fopen("TrieBinary", "r+");
    fseek(fp, 0, SEEK_SET);
    fread(&end, sizeof(unsigned short int), 1, fp);
    printf("%d\n\n", end);
    //end = 1;

    fpStack = fopen("TrieStack", "r+");
    fseek(fpStack, 0, SEEK_SET);
    fread(&availableTop, sizeof(unsigned short int), 1, fpStack);
    printf("%d\n\n", availableTop);
    //availableTop = 0;
    fread(availableStack, sizeof(short int) * availableTop, 1, fpStack);

    fpCnt=fopen("TrieCnt","r+");
    fread(cnt,sizeof(short int)*(1<<15),1,fpCnt);

    //memset(trie, 0, sizeof(trie));
    //memset(cnt, 0, sizeof(cnt));

    insertTrie("apple");
    insertTrie("apple");
    insertTrie("ball");
    insertTrie("apple");
    searchTrie("apple");
    searchTrie("ball");
    searchTrie("apple1");
    printStack();
    deleteTrie("apple");
    printStack();
    deleteTrie("ball");
    printStack();
    searchTrie("ball");
    deleteTrie("apple1");
    insertTrie("apple");
    printStack();
    searchTrie("apple");
    insertTrie("ball");
    printStack();
    // insertTrie("car");
    // insertTrie("cars");
    // insertTrie("balls");
    // insertTrie("car");
    // searchTrie("car");
    // searchTrie("cars");
    // searchTrie("balls");
    // printStack();
    // deleteTrie("cars");
    // printStack();
    // deleteTrie("ball");
    // printStack();
    // searchTrie("ball");
    // deleteTrie("apple1");
    // insertTrie("ball");
    // printStack();
    // searchTrie("cars");
    // insertTrie("ball");
    // printStack();

    fseek(fpCnt, 0, SEEK_SET);
    fwrite(cnt,sizeof(short int)*(1<<15),1,fpCnt);
    fclose(fpCnt);
    
    fseek(fpStack, 0, SEEK_SET);
    fwrite(&availableTop, sizeof(unsigned short int), 1, fpStack);
    // printf("%d\n\n", availableTop);
    fwrite(availableStack, sizeof(short int) * availableTop, 1, fpStack);
    fclose(fpStack);

    fseek(fp, 0, SEEK_SET);
    fwrite(&end, sizeof(unsigned short int), 1, fp);
    fclose(fp);
    return 0;
}
