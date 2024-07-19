import java.nio.file.*;
import java.io.IOException;

public class FileWatcher {
    public static void main(String[] args) {
        // 監視対象のファイルパスを指定
        Path path = Paths.get("/home/lucifer/d/app/myapp/tool_handmade/a.txt");  
        watchFile(path);
    }

    public static void watchFile(Path path) {
        try {
            WatchService watchService = FileSystems.getDefault().newWatchService();
            // ファイルの親ディレクトリを監視対象として登録
            path.getParent().register(watchService, StandardWatchEventKinds.ENTRY_MODIFY);

            System.out.println("Watching file: " + path.getFileName());

            while (true) {
                // ファイルの変更があるまで待機
                WatchKey key = watchService.take(); 
                for (WatchEvent<?> event : key.pollEvents()) {
                    WatchEvent.Kind<?> kind = event.kind();

                    if (kind == StandardWatchEventKinds.OVERFLOW) {
                        continue;
                    }

                    WatchEvent<Path> ev = (WatchEvent<Path>) event;
                    Path fileName = ev.context();

                    // 変更されたファイルが監視対象のファイルであるか確認
                    if (fileName.equals(path.getFileName())) {
                        System.out.println("file changed");
                    }
                }
                // 次の監視のためにキーをリセット
                key.reset(); 
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}
