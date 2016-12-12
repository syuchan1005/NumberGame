#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "sqlite3.h"

void getNowTime(char *);

void main() {
	char name[64], *errMessage = NULL, nowTime[20];
	int digit, answer, input, count = 0, i;
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;

	// ①説明表示
	printf("説明:\n");
	printf("指定された桁数内の乱数を生成するのでそれを当てて下さい。\n");
	printf("言い当てるまで何回でも指定できます。\n");
	printf("最後に言い当てるまでにかかった回数を表示します。\n\n");

	// ②必要な情報の入力
	printf("名前を入力してください: ");
	scanf_s("%s", &name, 64);
	printf("桁数を決めてください: ");
	scanf_s("%d", &digit);
	digit = (int)pow(10, digit);

	// ③乱数生成
	srand(time(NULL));
	answer = rand() % digit;
	printf("Result: %d\n", answer);

	// ④数の入力
	while (1) {
		printf("数を当てて下さい: ");
		scanf_s("%d", &input);
		count++;
		if (input == answer) break;

		// ⑤ヒントの表示
		input = abs(answer - input);
		if (input >= (answer / 2)) {
			printf("まったくの当て外れで～す\n");
		}
		else {
			if (rand() % 2 == 0) {
				digit = (int)log10((double)input) + 1;
				printf("%d桁以上(%1.f)位差があります\n", digit, floor(input / pow(10, digit - 1) + 0.5) * pow(10, digit - 1));
			}
			else {
				printf("ちなみに%sなんですよ～\n", (answer % 2 == 0) ? "偶数" : "奇数");
			}
		}
	}

	// ⑥成績表示
	printf("%sさんは%d回で当てました！\n", name, count);

	// ⑦ 作者表示
	printf("created by syuchan\n");

	// 拡張１
	getNowTime(nowTime);
	if (sqlite3_open("data.sqlite", &db) != SQLITE_OK) {
		printf("SQLiteファイルの読み込みに失敗しました");
	}
	else {
		if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS history(id INTEGER PRIMARY KEY AUTOINCREMENT,clearTime TEXT NOT NULL,name TEXT NOT NULL,count INTEGER NOT NULL)", NULL, NULL, &errMessage) != SQLITE_OK) {
			printf("%s\n", errMessage);
			sqlite3_free(errMessage);
			errMessage = NULL;
		}
		if (sqlite3_prepare_v2(db, "INSERT INTO history(clearTime,name,count) VALUES (?,?,?)", 128, &stmt, NULL) == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, nowTime, strlen(nowTime), SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 3, count);
			while (SQLITE_DONE != sqlite3_step(stmt)) {}
		}
		sqlite3_finalize(stmt);
		printf("-----RANKING-----\n");
		if (sqlite3_prepare_v2(db, "SELECT * FROM history ORDER BY count ASC LIMIT 5", 128, &stmt, NULL) == SQLITE_OK) {
			for (i = 1; sqlite3_step(stmt) == SQLITE_ROW; i++) {
				printf("%d: %s %d (%s)\n", i, (char *)sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3), (char *)sqlite3_column_text(stmt, 1));
			}
		}
		sqlite3_finalize(stmt);
		if (sqlite3_close(db) != SQLITE_OK) {
			printf("SQLiteファイルの終了に失敗しました");
		}
	}
	system("PAUSE");
}

void getNowTime(char *str) {
	time_t now = time(NULL);
	struct tm *pnow = localtime(&now);
	sprintf(str, "%04d/%02d/%02d %02d:%02d:%02d", pnow->tm_year + 1900, pnow->tm_mon + 1, pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
}