static void test_api(const char*);

static void init_app(host_info_t* host_info) {
  const int MAX_SIZE = 2048;
  char local_response_buffer[BUFFER_MAX_SIZE] = {0};

  esp_http_client_config_t config = {
    .host = host_info->host,
    .port = host_info->port,
    .path = "/app/init",
    .method = HTTP_METHOD_POST,
    .user_data = local_response_buffer,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    M5.Lcd.println("Failed to init.");
    return;
  }

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    M5.Lcd.println("Start application");
  }
  else {
    M5.Lcd.print(esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
}

static void start_app(host_info_t* host_info) {
  const int MAX_SIZE = 2048;
  char local_response_buffer[BUFFER_MAX_SIZE] = {0};

  esp_http_client_config_t config = {
    .host = host_info->host,
    .port = host_info->port,
    .path = "/app/start",
    .method = HTTP_METHOD_POST,
    .user_data = local_response_buffer,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    M5.Lcd.println("Failed to init.");
    return;
  }

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    M5.Lcd.println("Start application");
  }
  else {
    M5.Lcd.print(esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
}

static void stop_app(host_info_t* host_info) {
  char local_response_buffer[BUFFER_MAX_SIZE] = {0};

  esp_http_client_config_t config = {
    .host = host_info->host,
    .port = host_info->port,
    .path = "/app/stop",
    .method = HTTP_METHOD_POST,
    .user_data = local_response_buffer,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    M5.Lcd.println("Failed to init.");
    return;
  }

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    M5.Lcd.println("Stop application.");
  }
  else {
    M5.Lcd.print(esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
}

static void restore_app(host_info_t* host_info) {
  char local_response_buffer[BUFFER_MAX_SIZE] = {0};

  esp_http_client_config_t config = {
    .host = host_info->host,
    .port = host_info->port,
    .path = "/app/restore",
    .method = HTTP_METHOD_POST,
    .user_data = local_response_buffer,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    M5.Lcd.println("Failed to init.");
    return;
  }

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    // M5.Lcd.println("Restore application.");
  }
  else {
    M5.Lcd.print(esp_err_to_name(err));
  }
  esp_http_client_cleanup(client);
}

static void migrate(host_info_t* from_host, host_info_t* to_host) {
  char local_response_buffer[BUFFER_MAX_SIZE] = {0};

  char query_str[128] = "host=";
  strcat(query_str, to_host->host);
  strcat(query_str, "&port=");
  char num[10];
  itoa(to_host->port, num, 10);
  strcat(query_str, num);

  esp_http_client_config_t config = {
    .host = from_host->host,
    .port = from_host->port,
    .path = "/app/migrate",
    .query = query_str,
    .method = HTTP_METHOD_POST,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    M5.Lcd.println("Failed to init.");
    return;
  }

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    M5.Lcd.println("Success migration");
  }
  else {
    M5.Lcd.print(esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
}
void migrate_flow(host_info_t* from, host_info_t* to) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);

  // マイグレーション
  stop_app(from);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  delay(200);

  migrate(from, to);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  delay(200);

  restore_app(to);
}
