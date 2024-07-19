import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.file.*;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class LogWatcher {

    private static final String LOG_FILE_PATH = "/var/log/auth.log";
    private static final String WEBHOOK_URL = "https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J";
    private static final int RATE_LIMIT_WAIT = 2000; // 2秒待機

    public static void main(String[] args) {
        Path logDir = Paths.get("/var/log");
        try (WatchService watchService = FileSystems.getDefault().newWatchService()) {
            logDir.register(watchService, StandardWatchEventKinds.ENTRY_MODIFY);

            System.out.println("Watching " + LOG_FILE_PATH + " for changes...");

            while (true) {
                WatchKey key = watchService.take();
                for (WatchEvent<?> event : key.pollEvents()) {
                    WatchEvent.Kind<?> kind = event.kind();

                    if (kind == StandardWatchEventKinds.OVERFLOW) {
                        continue;
                    }

                    WatchEvent<Path> ev = (WatchEvent<Path>) event;
                    Path fileName = ev.context();

                    if (fileName.toString().equals("auth.log")) {
                        System.out.println(LOG_FILE_PATH + " has been modified!");
                        processLogFile();
                    }
                }

                boolean valid = key.reset();
                if (!valid) {
                    break;
                }
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private static List<String> readNewLines() {
        List<String> newLines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(LOG_FILE_PATH))) {
            String line;
            while ((line = reader.readLine()) != null) {
                newLines.add(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return newLines;
    }

    private static void processLogFile() {
        List<String> newLines = readNewLines();
        for (String line : newLines) {
            if (!line.contains("[preauth]") && !line.contains("Invalid user")) { // フィルタリング
                System.out.println("New log entry: " + line);
                sendToDiscord(line);
            }
        }
    }

    private static void sendToDiscord(String message) {
        try {
            String jsonPayload = String.format("{\"content\": \"%s\"}", message);

            URL url = new URL(WEBHOOK_URL);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("POST");
            connection.setRequestProperty("Content-Type", "application/json");
            connection.setDoOutput(true);

            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = jsonPayload.getBytes("utf-8");
                os.write(input, 0, input.length);
            }

            int responseCode = connection.getResponseCode();
            System.out.println("Response Code: " + responseCode);
            if (responseCode == HttpURLConnection.HTTP_NO_CONTENT) {
                System.out.println("Message sent successfully");
            } else if (responseCode == 429) { // Too Many Requests
                System.out.println("Rate limit hit, waiting...");
                TimeUnit.MILLISECONDS.sleep(RATE_LIMIT_WAIT); // 指定された時間待機
                sendToDiscord(message); // リトライ
            } else {
                System.out.println("Failed to send message");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
