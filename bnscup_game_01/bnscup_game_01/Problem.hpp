#pragma once

struct Problem
{
	String id;			 // 問題ID
	String author;		 // 作者名
	String source;		 // 出典
	String text;		 // 表示する句
	bool hasKigo;		 // 季語を持つか
	int32 kigoStart;	 // 季語開始インデックス（0-based）
	int32 kigoEnd;		 // 季語終了インデックス
	String kigo;		 // 季語文字列
	String explanation;	 // 正解時の解説テキスト
	Array<String> tags;	 // メタ情報タグ

	bool isValid() const;  // 範囲検証
};

