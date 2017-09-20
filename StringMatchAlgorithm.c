/*
描述：有一个文本串S，和一个模式串P，现在要查找P在S中的位置，怎么查找呢？
方法：
1、暴力搜索
   假设现在文本串S匹配到 i 位置，模式串P匹配到 j 位置，则有：
   如果当前字符匹配成功（即S[i] == P[j]），则i++，j++，继续匹配下一个字符；
   如果失配（即S[i]! = P[j]），令i = i - (j - 1)，j = 0。相当于每次匹配失败时，i 回溯，j 被置为0。
   KMP的整体时间复杂度为O(n^2)。
2、KMP算法（Next[]）
   假设现在文本串S匹配到 i 位置，模式串P匹配到 j 位置，则有：
   如果j = -1，或者当前字符匹配成功（即S[i] == P[j]），都令i++，j++，继续匹配下一个字符；
   如果j != -1，且当前字符匹配失败（即S[i] != P[j]），则令 i 不变，j = next[j]。
   此举意味着失配时，模式串P相对于文本串S向右移动了j - next [j] 位。换言之，当匹配失败时，
   模式串向右移动的位数为：失配字符所在位置 - 失配字符对应的next 值，即移动的实际位数为：
   j - next[j]，且此值大于等于1。
   KMP的整体时间复杂度为O(m + n)。
3、BM算法（全称为Boyer-Moore算法）
   该算法从模式串的尾部开始匹配，且拥有在最坏情况下O(N)的时间复杂度。在实践中，比KMP算法的实际效能高。
   BM算法最坏情况下O(N)的时间复杂度。
4、Sunday算法（跟BM算法很相似）
   只不过Sunday算法是从前往后匹配，在匹配失败时关注的是文本串中参加匹配的最末位字符的下一位字符。
   如果该字符没有在模式串中出现则直接跳过，即移动位数 = 匹配串长度 + 1；
   否则其移动位数 = 模式串中最右端的该字符到末尾的距离+1。

字符串查找算法性能排名：
	Sunday算法 > BM算法 > KMP算法 > 库函数strstr()（暴力搜索）
	BM算法则往往比KMP算法快上3－5倍。

测试案例：(测试案例没分析清楚，还得好好考虑下)
（1）正常情况：
	src[] = "ABCDAB ABCDABCDABD"; des[] = "ABCDABD";
	src[] = "ABCDAB ABCDABCDABD"; des[] = "ABCDABE";
	src[] = "ABCDAB ABCDABCDABD"; des[] = "ABCDABC";
（2）不正常情况：

（3）边界情况：

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHAR_SIZE 256	//一个字符8位 最大256种

//暴力破解法
int Violent_Search(const char *mainStr, const char *subStr);
//KMP匹配法
int Kmp_Search(const char *mainStr, const char *subStr, const int next[]);
void GetNext(const char *subStr, int next[]);//求解Next[]
void GetNext2(const char *subStr, int next[]);//求解Next[]，相对于GetNext求得的数组，这个数组让KMP比较次数少些
//BM算法(Boyer-Moore算法)
int Boyer_Moore_Search(const char *mainStr, const char *subStr);
//Sunday算法
int Sunday_Search(const char *mainStr, const char *subStr, const int *charStep);
void getCharStep(int *charStep, const char *subStr);//获取所有字符对应的移动步长存入charStep[]中
int str_sunday_search(const char *sur, const char *tar);

int main()
{
	int i, position = 0;//计数，返回位置
	int next[100];//KMP算法需要的Next数组
	int charStep[MAX_CHAR_SIZE];//Sunday算法需要的charStep数组
//	char src[] = "ABCDAB ABCDABCDABD";//目标串
//	char des[] = "ABCDABC";//模式串
//	char src[] = "ABCDABCDABEABCDABD";//目标串
//	char des[] = "ABCDABE";//模式串
	char src[] = "ABCDAB ABCDABCDABD";//目标串
	char des[] = "ABCDABD";//模式串

	printf("Violent algorithm to match string!\n");
	printf("src: %s\n", src);
	printf("des: %s\n", des);
	position = Violent_Search(src, des);
	printf("des is in: %d\n\n\n", position);

	printf("KMP algorithm to match string!\n");
	GetNext(des, next);
	printf("src: %s\n", src);
	printf("des: %s\n", des);
	for (i = 0; i < strlen(des); i++)
		printf("next[%d]=%d  ",i , next[i]);
	printf("\n");
	position = Kmp_Search(src, des, next);
	printf("des is in: %d\n\n\n", position);

	printf("BM algorithm to match string!\n");
	printf("src string: %s\n", src);
	printf("des string: %s\n", des);
	printf("BM算法暂时没有时间调试程序!调试过的小伙伴可以共享下!\n");
	position = Boyer_Moore_Search(src, des);
	printf("des string is in: 还没有做出来\n\n\n");

	printf("Sunday algorithm to match string!\n");
	printf("src string: %s\n", src);
	printf("des string: %s\n", des);
	getCharStep(charStep, des);
	for (i = 0; i < strlen(des); i++)
		printf("charStep[%c]=%d  ", (unsigned char)des[i], charStep[(unsigned char)des[i]]);
	printf("\n");
	position = Sunday_Search(src, des, charStep);
	printf("des string is in: %d\n\n\n", position);

	return 0;
}


//暴力破解法
int Violent_Search(const char *mainStr, const char *subStr)
{
	int mainStrLen = strlen(mainStr);//目标串的长度
	int subStrLen = strlen(subStr);//模式串的长度，不计入"\0"
	int main_i = 0;//目标串现在匹配的位置，0 - （mainStrLen-1）
	int sub_j = 0;//模式串现在匹配的位置，0 -（subStrLen-1）

	while ((main_i < mainStrLen) && (sub_j < subStrLen))
	{
		if (mainStr[main_i] == subStr[sub_j])//如果当前字符匹配成功
		{   
			main_i++;
			sub_j++;//接着匹配下一个字符
		}
		else//如果当前字符失配，i回溯，j=0。
		{  
			main_i = main_i - sub_j + 1;
			sub_j = 0;
		}
	}

	if (sub_j == subStrLen)//匹配成功，返回模式串p在文本串s中的位置
		return main_i - sub_j;
	else//匹配失败，目标串已经查找完毕
		return -1;
}

//KMP匹配法
int Kmp_Search(const char *mainStr, const char *subStr, const int next[])
{
	int mainStrLen = strlen(mainStr);//目标串的长度
	int subStrLen = strlen(subStr);//模式串的长度，不计入"\0"
	int main_i = 0;//目标串现在匹配的位置，0 - （mainStrLen-1）
	int sub_j = 0;//模式串现在匹配的位置，0 -（subStrLen-1）

	while ((main_i < mainStrLen) && (sub_j < subStrLen))
	{
		//如果sub_j = -1，或者当前字符匹配成功（即mainStr[main_i] == subStr[sub_j]），都令main_i++，sub_j++      
		if ((sub_j == -1) || (mainStr[main_i] == subStr[sub_j]))
		{
			main_i++;
			sub_j++;
		}
		else//如果sub_j != -1，且当前字符匹配失败（即mainStr[main_i] != subStr[sub_j]），则令main_i不变，sub_j = next[sub_j]
		{
			sub_j = next[sub_j];//next[j]即为j所对应的next值    
		}
	}
	if (sub_j == subStrLen)
		return main_i - sub_j;
	else
		return -1;
}
//普通的next数组求法
void GetNext(const char *subStr, int next[])
{
	int subStrLen = strlen(subStr);//模式串的长度
	int Prefix_i = -1;//模式串前缀
	int suffix_j = 0;//模式串后缀,0 - (subStrLen-1)
	next[0] = -1;//next数组第一个数永远是-1

	while (suffix_j < (subStrLen - 1))
	{
		//subStr[Prefix_i]表示前缀，subStr[suffix_j]表示后缀
		if ((Prefix_i == -1) || (subStr[Prefix_i] == subStr[suffix_j]))
		{
			++Prefix_i;
			++suffix_j;
			next[suffix_j] = Prefix_i;
		}
		else
		{
			Prefix_i = next[Prefix_i];
		}
	}
}
//优化过后的next数组求法 
//相对于GetNext求得的数组，这个数组让KMP比较次数少些
void GetNext2(const char *subStr, int next[])
{
	int subStrLen = strlen(subStr);//模式串的长度
	int Prefix_i = -1;//模式串前缀
	int suffix_j = 0;//模式串后缀,0 - (subStrLen-1)
	next[0] = -1;//next数组第一个数永远是-1

	while (suffix_j < (subStrLen - 1))
	{
		//subStr[Prefix_i]表示前缀，subStr[suffix_j]表示后缀
		if ((Prefix_i == -1) || (subStr[Prefix_i] == subStr[suffix_j]))
		{
			++Prefix_i;
			++suffix_j;

			//较普通的next数组求法，改动在下面4行  
			if (subStr[Prefix_i] != subStr[suffix_j])	//如果是Prefix_i==-1
				next[suffix_j] = Prefix_i;
			else  //如果是subStr[Prefix_i] == subStr[suffix_j]
				next[suffix_j] = next[Prefix_i];
		}
		else
		{
			Prefix_i = next[Prefix_i];
		}
	}
}

//BM法
int Boyer_Moore_Search(const char *mainStr, const char *subStr)
{
	return -1;
}

//Sunday算法
//mainStr为目标串指针
//subStr为模式串指针
//charStep[]为所有字符对应的移动步长
//返回值为模式串在目标串中的位置0 -（mainStrLen-1），返回-1则表示没有找到模式串
int Sunday_Search(const char *mainStr, const char *subStr, const int *charStep)
{
	int mainStrLen = strlen(mainStr);//目标串的长度
	int subStrLen = strlen(subStr);//模式串的长度，不计入"\0"
	int main_i = 0;//目标串现在匹配的位置，0 - （mainStrLen-1）
	int sub_j = 0;//模式串现在匹配的位置，0 -（subStrLen-1）
	char firstRightChar;//失配时，模式串尾符号对应目标串的位置的下一个位置的值

	while (main_i<mainStrLen)
	{
		int temp = main_i;//保存大串每次开始匹配的起始位置，便于移动指针
		while (sub_j<subStrLen)//未匹配完模块串
		{
			if (mainStr[main_i] == subStr[sub_j])//如果匹配
			{
				main_i++;
				sub_j++;
			}
			else//如果不匹配
			{
				//如果匹配范围外已经找不到右侧第一个字符，则匹配失败 
				if (temp + subStrLen >= mainStrLen)
					return -1;
				//否则移动步长，重新匹配。获取失配时，模式串尾符号对应目标串的位置的下一个位置的值
				firstRightChar = mainStr[temp + subStrLen];
				temp += charStep[(unsigned char)firstRightChar];
				main_i = temp;
				sub_j = 0;//退出本次失败匹配 重新一轮匹配
				break;
			}
		}
		if (sub_j == subStrLen)//匹配完模块串，返回开始匹配的位置
			return main_i - subStrLen;
	}
	return -1;
}
/*
获取ASCII码0-255号字符对应的目标串移动步长，将其存放在charStep[]中。
存入规则：
如果目标串中匹配字符的右侧一个字符没在模式串中，目标串移动步长=整个模式串的距离+1
如果目标串中匹配范围内的右侧一个字符在模式串中，目标串移动步长=模式串长度-这个字符在模式串中的位置
*/
void getCharStep(int charStep[], const char *subStr)
{
	int i;
	int subStrLen = strlen(subStr);

	//先默认256个字符都不存在模式串中，都存入：目标串移动步长=整个模式串的距离+1
	for (i = 0; i<MAX_CHAR_SIZE; i++)
		charStep[i] = subStrLen + 1;

	//从左向右扫描一遍，修改模式串中的字符对应的值为：目标串移动步长=模式串长度-这个字符在模式串中的位置
	//从左向右是必须的，以便让后面相同的字符更新移动步长
	for (i = 0; i<subStrLen; i++)
	{
		charStep[(unsigned char)subStr[i]] = subStrLen - i;
	}
}
