---
Title: Counter Strike 1.6 - Ubuntu VPS Server 建立伺服器並且廿四小時開放伺服器
Date: 2021-12-11T23:25:24+0800
Tags: Server, 指令, Games, diu,  lee,  lo,  mo
Category: Counter-Strike 1.6, Sex, Peter,  is,  on9,  jai
---

# 前言
我地如果要喺cs1.6 咁公開一個IP俾唔同國家既人入黎，我地需要開放自己部機既Public IP 俾人入 (Wan Network)。
但係用自己部機黎做Server Host 實在太唔安全，所以今次我地會用 VPS Hosting 做我地既 Server Side，並且係24小時開放。

講完咁多廢話，我地正式開始教學!

Q : 止於點樣申請VPS ?
A : 遲D我再寫一篇教大家點申請 VPS Hosting 既 Post :3

# 安裝 SSH

我地用 SSH 登入我地既 Server (基本上所有Linux 系統 都會本身幫你裝左 ssh)，如果冇就打呢句裝。
```
sudo apt-get install ssh
```

## 登入 SSH

```shell
ssh root@your_ipv4_address
```

之後第一次入佢會問你 yes 定 no

咁我地緊係打 yes 啦 (唔打yes點入個server呀屌你?)

打 `yes`

然後我地要將你既 Default Reject Port22 登入既開返做 Accept
呢個動作係為左唔好令你日後入返個 VPS Server 會 Reject左你既登入

## SSH Port Accept
```shell
iptables -A INPUT -p tcp --dport 22 -j ACCEPT
```

# Steamcmd 操作
---

## 安裝 `steamcmd`
```shell
sudo apt-get update -y #UPDATE APT
```
```shell
sudo add-apt-repository multiverse
```
```shell
sudo dpkg --add-architecture i386
```
```shell
sudo apt-get update -y
```
```shell
sudo apt-get dist-upgrade -y 
```
```shell
sudo apt install libsdl2-2.0-0:i386 -y
```
```shell
sudo apt install steamcmd -y
```
```shell
sudo apt-get update -y
```
```shell
sudo apt install lib32gcc1 -y
```

## 開啟 Steamcmd

```shell
steamcmd #開你岩岩裝完既steamcmd
```

## 登入 Anonymous
```shell
login anonymous #登入 Anomyous
```

## 下載 steamcmd 資料

呢度要打兩次，第一次會 Error，呢個係 steamcmd 問題，只要打多次就得。

```shell
app_update 90 validate #數字90 係代表 cs1.6
app_update 90 validate
```

## 退出 steamcmd

當你完成下載後，輸入以下指令退出 `steamcmd`。

```shell
exit
```

# 下載 wget

以下操作需要使用指令下載檔案，我既示範會用 `wget`。

因為我地呢個位係冇得用 browser 下載，請你準備定喺個網站度 copy files link，又或者你跟我下面咁做就得。

```shell
sudo apt-get install weget -y
```

## wget 下載所需檔案

解壓完會有個 `addons` folder，呢度記得要順住次序咁做，
`base` > `cstrike` > `metamod`。

```shell
wget https://www.amxmodx.org/release/amxmodx-1.8.2-base-linux.tar.gz
tar zxvf amxmodx-1.8.2-base-linux.tar.gz

wget https://www.amxmodx.org/release/amxmodx-1.8.2-cstrike-linux.tar.gz
tar zxvf amxmodx-1.8.2-cstrike-linux.tar.gz

wget https://www.amxmodx.org/release/metamod-1.21.1-am.zip;

sudo apt-get install unzip

unzip metamod-1.21.1-am.zip
```

# 檔案操作

以下操作會進行放置和修改檔案。

---

複製 `addons` 去 `~/Steam/steamapps/common/Half-Life/cstrike/`
```shell
cp -r addons/ ~/Steam/steamapps/common/Half-Life/cstrike/
```

## 檔案修改

修改 `~/Steam/steamapps/common/Half-Life/cstrike/liblist.gam` 檔案

```shell
nano ~/Steam/steamapps/common/Half-Life/cstrike/liblist.gam #使用nano修改
```

搵 `gamedll` 個行 再改做
```shell
gamedll_linux "addons\metamod\dlls\metamod.so"
```

喺 `~/Steam/steamapps/common/Half-Life/cstrike/addons/metamod/plugins.ini`

加一行
```
linux addons\amxmodx\dlls\amxmodx_mm.co
```

# 防火牆設置

以下操作會為主機開放 port。

---

## ufw 防火牆安裝

我呢邊會示範用 ufw 黎 setup

如果你本身係冇裝到，輸入以下指令安裝。
```shell
sudo apt install ufw -y
```

## 啟動 ufw 防火牆

```shell
sudo ufw enable #開啟ufw firewall
```

## 開放 UDP Port

```shell
sudo ufw allow 27015/udp #開放 27015 UDP Port
```

# 安裝 Screen

用 `Screen` 係為左喺 Background run Server，
防止你熄左機就會熄左 Server。

```shell
sudo apt-get install screen -y
```

## 開 Server

我地會用 Screen 開我地既 Server。

入去 `~/Steam/steamapps/common/Half-Life/`

```shell
cd ~/Steam/steamapps/common/Half-Life/
```

```shell
screen -dmS hlds ./hlds_run -game cstrike +map zm_house_ykh_ysh_v4 -port 27015 +maxplayers 32 -insecure exec server.cfg
```

## 入去 Server console
```shell
screen -r hlds
```

## 暫時退出而加既 Screen Session

鍵盤同時撳呢三粒制 : <kbd>Ctrl</kbd>+<kbd>A</kbd>+<kbd>D</kbd>


## 入返 Screen Session

我地首先要查下你岩岩出左既 Session Name 係乜 (上面因為用左 -dmS set 名，所以你只需要打 `screen -r hlds`)

```shell
screen -list
```

見到之後再打
```shell
screen -r [你見到既名]
```

# 開啟 CS 進入你的伺服器

開啟你的 Counter-Strike 1.6 進入你的伺服器。

## 進入伺服器方法一
- 開你既 cs1.6
- 撳 Keyboard ` 打 :

```shell
connect [你既 VPS Server IP]
```

## 進入伺服器方法一

去你既 Server List 加 你既 Server IP。