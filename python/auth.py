import subprocess
import requests
import time

def send_to_discord(message, webhook_url="https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J"):
    response = requests.post(webhook_url, json={"content": message})
    if response.status_code != 204:
        print(f"Failed to send message: {response.status_code}")

def get_auth_log():
    result = subprocess.run(['cat', '/var/log/auth.log'], stdout=subprocess.PIPE)
    auth_log = result.stdout.decode('utf-8').splitlines()
    return auth_log



if __name__ == "__main__":
    previous_log = get_auth_log()
    id = "untiburiburi"
    id_single = "untiburiburi"
    
    while True:
        current_log = get_auth_log()
        if current_log is None :
            continue
        new_entries = [line for line in current_log if line not in previous_log]
        if new_entries == []:
            time.sleep(5)
            continue

        previous_log = current_log
        
        for i in new_entries:
            # たまにconnection fromが二回くるのは、barner errorのところにもあるから。くわしくはwireguard.txt。
            if i.find("]: Connection from") != -1:
                # 自宅サーバーからのアクセスの場合はログを送信しない。connection from 20.0.0.1だった場合、sshd[id]のidの中身を保存しておいて、そのidのaccept keyなら送信しない。idには"[123434]:"が入っている。スライスの終点がめんどくさいのであとで[]の中を取り出すようにしよう。
                if i.find("192.168.") != -1:
                    id = i.split()[4][4:]
                else:
                    send_to_discord(f'{i.split()[2]} Connection from...')
            elif i.find("Accepted key") != -1:
                # idは20.0.0.1のsshd[]で、id_singleはaccept keyが二回送るのを防ぐためのもの。auth.logを見るとわかるが、なぜかaccept keyのログが二回残る。二回認証しているのだろうか。１回discordにおくったら、そのsshd[id]のidを記録しておいて、またそのidのaccept keyがきたらスルーする。
                if i.find(id) == -1 and i.find(id_single) == -1:
                    id_single = i.split()[4][4:]
                    send_to_discord(f'{i.split()[2]} {" ".join(i.split()[5:7])} {i.split()[11]}')
            elif i.find("error:") != -1:
                send_to_discord(f'{i.split()[2]} {" ".join(i.split()[5:])}')

        #time.sleep(1)

