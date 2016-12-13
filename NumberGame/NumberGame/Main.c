#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "sqlite3.h"

void main() {
	char name[20], *errMessage = NULL;
	int digit, answer, input, count = 0, i;
	clock_t start, end;
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;

	// ①説明表示
	printf("説明:\n");
	printf("指定された桁数内の乱数を生成するのでそれを当てて下さい。\n");
	printf("言い当てるまで何回でも指定できます。\n");
	printf("最後に言い当てるまでにかかった回数を表示します。\n\n");

	// ②必要な情報の入力
	printf("名前を入力してください: ");
	scanf_s("%s", &name, 20);
	printf("桁数を決めてください: ");
	scanf_s("%d", &digit);
	digit = (int)pow(10, digit);

	// ③乱数生成
	srand(time(NULL));
	answer = rand() % digit;
	printf("Result: %d\n", answer);

	// ④数の入力
	start = clock();
	while (1) {
		printf("数を当てて下さい: ");
		scanf_s("%d", &input);
		count++;
		if (input == answer) break;

		// ⑤ヒントの表示
	}

	// ⑥成績表示
	end = clock();
	printf("%sさんは%d回で当てました！\n", name, count);
	printf("経過時間%d[ms]\n", end - start);

	// ⑦ 作者表示
	printf("created by syuchan\n");

	// 拡張
	if (sqlite3_open("data.sqlite3", &db) != SQLITE_OK) {
		printf("SQLiteファイルの読み込みに失敗しました");
	}
	else {
		// 拡張１
		if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS history(id INTEGER PRIMARY KEY AUTOINCREMENT,clearTime INTEGER NOT NULL,name TEXT NOT NULL,count INTEGER NOT NULL)", NULL, NULL, &errMessage) != SQLITE_OK) {
			printf("%s\n", errMessage);
			sqlite3_free(errMessage);
			errMessage = NULL;
		}
		if (sqlite3_prepare_v2(db, "INSERT INTO history(clearTime,name,count) VALUES (?,?,?)", 128, &stmt, NULL) == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, end - start);
			sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 3, count);
			while (SQLITE_DONE != sqlite3_step(stmt)) {}
		}
		sqlite3_finalize(stmt);
		// 拡張２
		printf("------------------------------RANKING-------------------------------\n");
		printf("%2s:\t%20s\t%8s\t%s\n", "順位", "名前", "当てるまでの回数", "経過時間[ms]");
		if (sqlite3_prepare_v2(db, "SELECT * FROM history ORDER BY count ASC LIMIT 3", 128, &stmt, NULL) == SQLITE_OK) {
			for (i = 1; sqlite3_step(stmt) == SQLITE_ROW; i++) {
				printf("%4d:\t%20s\t%16d\t%12d\n", i, (char *)sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 1));
			}
		}
		sqlite3_finalize(stmt);
		if (sqlite3_close(db) != SQLITE_OK) {
			printf("SQLiteファイルの終了に失敗しました");
		}
	}
	// system("PAUSE");
}

void print_hint(int answer, int input) {
	int digit, diff;
	diff = abs(answer - input);
	if (diff >= (answer / 2)) {
		printf("まったくの当て外れで～す\n");
	}
	else {
		if (rand() % 2 == 0) {
			digit = (int)log10((double)diff) + 1;
			printf("%d桁以上(%1.f)位差があります\n", digit, floor(diff / pow(10, digit - 1) + 0.5) * pow(10, digit - 1));
		}
		else {
			printf("ちなみに%sなんですよ～\n", (answer % 2 == 0) ? "偶数" : "奇数");
		}
	}
}
