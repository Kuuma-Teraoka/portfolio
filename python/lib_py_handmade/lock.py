
import os

# lockファイルを作成して、他のコンピュータにこのディレクトリを計算中であることを知らせる。open("x")で確認と作成を同時に行うことで確実にしている。
def create(simulation_dir): 
    lock_file = simulation_dir + "/" + "lock.bin"
    try:
        with open(lock_file, 'x'):
            return True
    except FileExistsError:
        return False

def remove(simulation_dir): 
    lock_file = simulation_dir + "/" + "lock.bin"
    if os.path.exists(lock_file):
        os.remove(lock_file)


