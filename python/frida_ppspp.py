
import frida
import subprocess
import struct
import time


def find_ppsspp_pid():
    result = subprocess.check_output(["pgrep", "-fl", "PPSSPPSDL"]).decode().strip()
    lines = result.split('\n')
    for line in lines:
        parts = line.split()
        if "PPSSPPSDL" in parts[-1]:
            return int(parts[0])
    raise Exception("PPSSPPのプロセスが見つかりません")

PSP_BASE = 0x08800000
HOST_BASE = 0x300000000
#0x300014000 # vmmap 83943 | grep -E "mapped|anon|heap|stack"でmapped file                 300014000-301804000    [ 23.9M   624K     0

def convert_psp_to_host(psp_address):
    return HOST_BASE + (psp_address - PSP_BASE)

psp_address = 0x09A44F3C
#psp_address = 0x08800000
host_address = convert_psp_to_host(psp_address)

ppsspp_pid = find_ppsspp_pid()
session = frida.attach(ppsspp_pid)
session.resume()  # これを入れないとPPSSPPが止まる

script = session.create_script("""
rpc.exports = {
    readmemory: function (address, length) {
        var buf = Memory.readByteArray(ptr(address), length);
        return buf;
    }
};
""")
script.load()



'''
# メモリダンプ実行
psp_start = 0x08800000
psp_end = 0x0A000000
psp_size = psp_end - psp_start
print(f"ダンプ対象: PSP RAM 0x{psp_start:X} - 0x{psp_end:X} (24MB)")
print(f"対応ホストアドレス: 0x{host_address:X} - 0x{host_address+psp_size:X}")

data = script.exports_sync.readmemory(host_address, psp_size)

# 16バイトごとに16進ダンプ表示
for i in range(0, len(data), 16):
    chunk = data[i:i+16]
    hex_chunk = ' '.join(f'{b:02X}' for b in chunk)
    print(f'0x{psp_start+i:08X}: {hex_chunk}')


'''



# ループして定期的に所持金を読む
try:
    while True:
        data = script.exports_sync.readmemory(host_address, 4)  # 4バイト（32bit）読む
        money = struct.unpack('<I', data)[0]  # little endianで整数に変換
        print(f"現在の所持金: {money}ゼニー")

        time.sleep(0.5)  # 0.5秒ごとに更新
except KeyboardInterrupt:
    print("終了します")


'''
size = 16
data = script.exports_sync.readmemory(host_address, size)

print(f"Read Memory (0x{psp_address:X} -> 0x{host_address:X}):")
print(data.hex())
'''


