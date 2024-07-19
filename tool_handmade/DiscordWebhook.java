import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class DiscordWebhook {
    public static void main(String[] args)
    {
        String message = "{\"content\": \"hello\"}";
        String url = "https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J" ;

        if(send_message(message, url) == 1)
            System.out.println("Message sent successfully");
        else
            System.out.println("Failed to send message");
    }

    static int send_message(String message, String webhookUrl)
    {
        try
        {
            // メッセージ内容をJSON形式で作成
            
            
            // URLオブジェクトの作成
            URL url = new URL(webhookUrl);
            
            // HTTPリクエストの設定
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("POST");
            connection.setRequestProperty("Content-Type", "application/json");
            connection.setDoOutput(true);
            
            // メッセージを送信
            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = message.getBytes("utf-8");
                os.write(input, 0, input.length);
            }
            
            // レスポンスを確認
            int responseCode = connection.getResponseCode();
            //System.out.println("Response Code: " + responseCode);

            if (responseCode == HttpURLConnection.HTTP_NO_CONTENT)
                return 1 ;
            else
                return 0 ;

        }
        catch (Exception e)
        {
            e.printStackTrace();
            return 0 ;
        }
    }
}
