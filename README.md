# ロボット演習 / Robotics Exercise

グループワークでのロボット演習プロジェクト用の共同開発リポジトリです。
This is a collaborative development repository for the robotics group work project.

---

## 📂 ディレクトリ構成 / Repository Structure

本リポジトリは、ロボットの設計、制御プログラム、シミュレーション、およびドキュメントを効率的に管理するため、以下のディレクトリ構成で運用します。

```text
├── docs/             # 設計書、ミーティング議事録、レポート、マニュアル等
├── hardware/         # 3D CADファイル (Fusion360/SolidWorks等)、回路図、BOM (部品表)
├── simulation/       # シミュレーション環境 (Gazebo, Webots, CoppeliaSim 等) の設定やモデル
├── src/              # ロボット制御、画像処理等のソースコード (Python, C++等)
├── .gitignore        # バージョン管理から除外するファイルの設定
├── LICENSE           # ライセンスファイル (MIT License)
└── README.md         # 本ファイル
```

---

## 👥 メンバー / Members

| 役割 / Role | 名前 / Name | 担当範囲 / Responsibility |
| :--- | :--- | :--- |
| **リーダー / Leader** | [名前 / Name] | 全体管理、統合 / Management & Integration |
| **制御・ソフトウェア** | [名前 / Name] | 制御アルゴリズム、センサ処理 / Control & Sensors |
| **ハードウェア・設計** | [名前 / Name] | 3D CAD設計、筐体製作 / CAD & Fabrication |
| **シミュレーション** | [名前 / Name] | シミュレーション環境構築 / Simulation & Testing |

---

## 🛠 開発環境とセットアップ / Setup & Installation

開発に必要な環境とセットアップ方法について記述します。

### 前提条件 / Prerequisites
- Git
- Python 3.10+ / C++ (ROS 2 等の環境に合わせて記述してください)
- (使用する場合) [PlatformIO](https://platformio.org/) または [Arduino IDE](https://www.arduino.cc/en/software)
- (使用する場合) [ROS 2 (Robot Operating System 2)](https://docs.ros.org/)

### クローン / Clone
```bash
git clone <GitHubのリポジトリURL>
cd robotics-exercise
```

---

## 🤝 共同開発のルール / Contribution Guidelines

グループで円滑に開発を進めるために、以下のルールを遵守してください。

### 1. ブランチ運用 / Branching Strategy
- **`main`**: 常に動作する安定版。直接コミットは禁止です。
- **`develop`**: 開発用ブランチ。機能開発の統合場所です。
- **`feature/○○`**: 各機能の開発用ブランチ。`develop` から分岐し、開発完了後に Pull Request (PR) を作成してください。
- **`hotfix/○○`**: 緊急バグ修正用ブランチ。

### 2. コミットメッセージの書き方 / Commit Messages
コミットメッセージは、何を変更したかが分かりやすいように以下の接頭辞（プレフィックス）を使用してください：

- `feat:` 新機能の追加
- `fix:` バグの修正
- `docs:` ドキュメントの変更（READMEなど）
- `style:` コードの意味に影響を与えない変更（改行、インデント調整など）
- `refactor:` リファクタリング（機能追加やバグ修正を伴わないコード整理）
- `test:` テストコードの追加・修正
- `chore:` ビルドプロセスの変更、依存関係のアップデートなど

例：`feat: 超音波センサによる障害物回避ロジックを追加`

### 3. プルリクエスト (PR) とレビュー / PR & Review
- `develop` ブランチへマージする際は、必ず Pull Request を作成してください。
- 動作検証（シミュレータまたは実機）を行い、他のメンバーのレビュー（最低1名の Approvol）を得てからマージしてください。

---

## 📄 ライセンス / License

本プロジェクトは **MIT License** の下で公開されています。詳細は [LICENSE](LICENSE) ファイルを参照してください。
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
