#pragma once

// 認定問題の段位定義
struct ProblemGrade
{
	static constexpr int32 Trainee = 0;	 // 特待生
	static constexpr int32 Master = 1;	 // 名人
	static constexpr int32 Expert = 2;	 // 達人
	static constexpr int32 Count = 3;	 // 段位認定問題の数
};

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

	// 段位システム用フィールド
	int32 grade = ProblemGrade::Trainee;  // 難易度（0=特待生, 1=名人, 2=達人）
	bool completed = false;		   // 正解済みフラグ
	String ruby;				   // フリガナ情報
	String rhythm;				   // リズム情報
	bool displayRuby = false;	   // フリガナ表示フラグ

	bool isValid() const;  // データの整合性チェック

   private:
	// 検証メソッドを細分化
	bool isBasicDataValid() const;	// 基本データの検証
	bool isKigoValid() const;		// 季語関連の検証
	bool isGradeValid() const;		// 段位の検証
	bool isRubyValid() const;		// フリガナの検証
	bool isRhythmValid() const;		// リズム情報の検証

   public:
	// デフォルトコンストラクタ
	Problem() : hasKigo(false), kigoStart(-1), kigoEnd(-1)
	{
	}

	String getSeason() const;
};
