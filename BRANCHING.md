# 分支维护说明

## 分支和远程仓库

| 名称 | 用途 |
| --- | --- |
| `upstream` | 原仓库 `https://github.com/tindy2013/subconverter.git` |
| `origin` | 本 fork `hkuc/subconverter` |
| `master` | 与 `upstream/master` 一致，不接收定制修改 |
| `dev` | 集成 DNS、模板、VLESS、Hysteria2 和缓存修复等定制修改 |
| `codex/*` 等临时分支 | 从 `dev` 创建，完成后通过 PR 合并回 `dev` |

GitHub 默认分支仍是 `master`。开发和下载定制源码时请选择 `dev`，PR 的目标分支也请选择 `dev`。

2026-11-05 整理分支时，`dev` 保留原 fork 的定制提交，并合入缓存修复提交 `f9441a5`。上游相对原 fork 新增的 9 个提交只同步到 `master`，尚未合入 `dev`，需要单独解决冲突并验证。

## 日常开发

```sh
git switch dev
git pull --ff-only origin dev
git switch -c codex/your-change
# 修改、验证、提交后推送，并向 hkuc/subconverter 的 dev 分支发起 PR。
git push -u origin codex/your-change
```

## 同步上游

先同步纯上游分支，再将上游更新合入开发分支：

```sh
git fetch upstream
git switch master
git merge --ff-only upstream/master
git push origin master

git switch dev
git pull --ff-only origin dev
git switch -c codex/sync-upstream
git merge master
# 解决冲突并完成测试后提交，向 dev 发起 PR。
git push -u origin codex/sync-upstream
```

如果 `master` 无法快进，先检查是否误提交了定制代码或上游改写了历史，不直接强制覆盖。分支整理时的历史重置是一次性操作，日常同步不需要强制推送。

## 构建和部署

`dev` 的推送和目标为 `dev` 的 PR 会运行构建工作流。下载与缓存回归测试另有独立工作流，相关源码变化时运行。

定制镜像必须从当前源码构建：

```sh
git switch dev
git pull --ff-only origin dev
docker build -f scripts/Dockerfile.source -t hkuc/subconverter:dev-<commit> .
```

该 Dockerfile 会先运行下载与缓存回归测试，再编译服务。现有运行目录应挂载到 `/base`，设置和缓存随之保留；切换前备份目录及镜像。

生产环境使用通过验证的提交对应的镜像标签，单独部署和回退。同步分支、切换本机分支和推送提交不会自动更新现有服务器容器。版本标签可能触发现有发布工作流，只在计划发布时创建。
