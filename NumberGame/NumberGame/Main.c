#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "sqlite3.h"

#define SQLiteName "data.sqlite3"

int get_digit(int);
int input_digit();
void print_hint(int, int);
void create_table();
void save_ranking(int, char*, int, int);
void print_ranking();

sqlite3 *db;

void main() {
	char name[20];
	int digit, answer = 1;
	int input,  count = 0;
	clock_t start, end;

	// 1 説明表示
	printf("遊び方:\n");
	printf("指定された桁数内の乱数を生成するのでそれを当てて下さい。\n");
	printf("言い当てるまで何回でも指定できます。\n");
	printf("最後に言い当てるまでにかかった回数を表示します。\n\n");

	// 2 必要な情報の入力
	printf("名前を入力してください: ");
	scanf_s("%s", &name, 20);
	digit = input_digit();

	// 3 乱数生成
	srand(time(NULL));
	do {
		if (answer == 0) answer = 1;
		answer = (answer * rand()) % digit;
		if (answer < 0) answer *= -1;
	} while (get_digit(digit) - 1 != get_digit(answer));
	printf("Result: %d\n", answer);

	// 4 数の入力
	start = clock();
	while (1) {
		printf("数を当てて下さい: ");
		scanf_s("%d", &input);
		count++;
		if (input == answer) break;
		// 5 ヒントの表示
		print_hint(answer, input);	
	}

	// 6 成績表示
	end = clock();
	printf("%sさんは%d回で当てました！\n", name, count);
	printf("経過時間%d[ms]\n", end - start);

	// 拡張
	if (sqlite3_open(SQLiteName, &db) != SQLITE_OK) {
		printf("SQLiteファイルの読み込みに失敗しました");
	}
	else {
		// 拡張１
		create_table();
		save_ranking(end - start, name, count, get_digit(answer));
		// 拡張２
		print_ranking();
		sqlite3_close(db);
	}

	// 7 作者表示
	printf("created by syuchan\n");

	system("PAUSE");
}

int get_digit(int num) {
	return log10(num) + 1;
}

int input_digit() {
	int digit;
	while (1) {
		printf("桁数を決めてください: ");
		scanf_s("%d", &digit);
		if (digit < 10) break;
		printf("指定できるのは9桁までです\n");
	}
	return (int)pow(10, digit);
}

void print_hint(int answer, int input) {
	int digit, diff;
	diff = abs(answer - input);
	if (diff >= (answer / 2)) {
		printf("まったくの当て外れで～す\n");
	}
	else {
		if (rand() % 2 == 0) {
			digit = get_digit(diff);
			printf("%d桁以上(%1.f)位差があります\n", digit, floor(diff / pow(10, digit - 1) + 0.5) * pow(10, digit - 1)); // 最上位桁 * 10^桁数
		}
		else {
			printf("ちなみに%sなんですよ～\n", (answer % 2 == 0) ? "偶数" : "奇数");
		}
	}
}

void create_table() {
	char* errMessage = NULL;
	if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS history(id INTEGER PRIMARY KEY AUTOINCREMENT,clearTime INTEGER NOT NULL,name TEXT NOT NULL,count INTEGER NOT NULL, digit INTEGER NOT NULL)"
			, NULL, NULL, &errMessage) != SQLITE_OK) {
		printf("%s\n", errMessage);
		sqlite3_free(errMessage);
		errMessage = NULL;
	}
}

void save_ranking(int clearTime, char* name, int count, int digit) {
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, "INSERT INTO history(clearTime,name,count,digit) VALUES (?,?,?,?)", 128, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, clearTime);
		sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 3, count);
		sqlite3_bind_int(stmt, 4, digit);
		while (SQLITE_DONE != sqlite3_step(stmt)) {}
	}
}

void print_ranking() {
	sqlite3_stmt *stmt;
	int i;
	printf("------------------------------RANKING-------------------------------\n");
	printf("%2s:\t%20s\t%8s\t%s\t%2s\n", "順位", "名前", "当てるまでの回数", "経過時間[ms]", "桁数");
	if (sqlite3_prepare_v2(db, "SELECT * FROM history ORDER BY count ASC LIMIT 3", 128, &stmt, NULL) == SQLITE_OK) {
		for (i = 1; sqlite3_step(stmt) == SQLITE_ROW; i++) {
			printf("%4d:\t%20s\t%16d\t%12d\t%4d\n", i, (char *)sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 4));
		}
	}
	sqlite3_finalize(stmt);
}