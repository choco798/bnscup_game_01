# D:/bnscup_game_01/bnscup_game_01/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(bnscup_game_01 LANGUAGES CXX)

# ソース自動列挙（ヘッダも含めておく）
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    "*.cpp" "*.hpp")

add_executable(${PROJECT_NAME} ${SRC})

# OpenSiv3D のインクルードパス（例）
# ここはあなたの環境に合わせて修正
include_directories(
    "C:/Users/takan/Documents/OpenSiv3D_0.6.16/include"
)

target_include_directories(bnscup_game_01 PRIVATE
  "C:/Users/takan/Documents/OpenSiv3D_0.6.16/include/ThirdParty"  # ThirdParty ディレクトリも追加
)

# MSVC 向け設定（コンパイルオプションをclangdに伝える）
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
