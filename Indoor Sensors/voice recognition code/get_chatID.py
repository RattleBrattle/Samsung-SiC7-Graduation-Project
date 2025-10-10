# File: get_telegram_chat_id.py
import requests
import sys

def get_chat_id(bot_token: str) -> None:
    """Fetch and print chat ID from the latest Telegram update."""
    url = f"https://api.telegram.org/bot{bot_token}/getUpdates"
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        data = response.json()
        
        if not data.get("ok"):
            print("Error: Invalid response from Telegram API.")
            return
        
        results = data.get("result", [])
        if not results:
            print("No messages found. Send a message to your bot first.")
            return
        
        chat_id = results[0]["message"]["chat"]["id"]
        print(f"✅ Your chat ID is: {chat_id}")
    except requests.exceptions.RequestException as e:
        print(f"HTTP error: {e}")
    except (KeyError, IndexError):
        print("Could not extract chat ID. Make sure your bot received a message.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python get_telegram_chat_id.py <BOT_TOKEN>")
        sys.exit(1)
    
    bot_token = sys.argv[1]
    get_chat_id(bot_token)
