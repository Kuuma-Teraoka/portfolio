import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.file.*;
import java.util.ArrayList;
import java.util.List;

public class LogWatcher {

    private static final String LOG_FILE_PATH = "/var/log/auth.log";

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
            System.out.println("New log entry: " + line);
            sendToDiscord(line);
        }
    }

    private static void sendToDiscord(String message) {
        try {
            String webhookUrl = "https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J";
            String jsonPayload = String.format("{\"content\": \"%s\"}", message);

            URL url = new URL(webhookUrl);
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
            } else {
                System.out.println("Failed to send message");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
