
import struct
import ctypes
from ctypes import *
import time

libc = ctypes.CDLL('/usr/lib/libc.dylib')

# mach API関数定義
task_for_pid = libc.task_for_pid
mach_vm_read_overwrite = libc.mach_vm_read_overwrite

task_for_pid.argtypes = [c_uint, c_int, POINTER(c_uint)]
mach_vm_read_overwrite.argtypes = [c_uint, c_ulonglong, c_ulonglong, c_void_p, POINTER(c_size_t)]

KERN_SUCCESS = 0

# PPSSPPのPIDを手動設定（要自分で確認）
ppsspp_pid = int(input("PPSSPPのPIDを入力してください: "))

def get_task_for_pid(pid):
    task = c_uint()
    result = task_for_pid(0, pid, byref(task))
    if result != KERN_SUCCESS:
        raise Exception(f"task_for_pid failed: {result} (SIPが有効だと失敗します)")
    return task.value

def read_memory(task, address, length):
    buffer = create_string_buffer(length)
    size = c_size_t(length)
    result = mach_vm_read_overwrite(task, address, length, buffer, byref(size))
    if result != KERN_SUCCESS:
        raise Exception(f"mach_vm_read_overwrite failed: {result}")
    return buffer.raw

def main():
    task = get_task_for_pid(ppsspp_pid)

    # ベースアドレス（デバッガから得たもの）
    psp_base_address = 0x0000000309008000

    # PSP上のモンスター体力アドレス（仮想アドレス）
    psp_health_address = 0x09F4F690

    # 実メモリアドレスに変換
    real_health_address = psp_base_address + (psp_health_address - 0x08800000)

    print(f"Real Health Address: 0x{real_health_address:X}")

    try:
        while True:
            # 4バイト（32bit）の体力を読む
            data = read_memory(task, real_health_address, 4)
            current_health = struct.unpack('<I', data)[0]  # Little-endian (PSPはリトルエンディアン)

            max_health_address = real_health_address + 4
            data = read_memory(task, max_health_address, 4)
            max_health = struct.unpack('<I', data)[0]

            print(f"Current Health: {current_health} / {max_health}")

            time.sleep(0.5)

    except KeyboardInterrupt:
        print("\n終了します")

if __name__ == '__main__':
    main()

