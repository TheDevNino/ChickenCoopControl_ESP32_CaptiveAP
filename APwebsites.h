const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Chicken Coop Control - Captive Portal</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background-color: #f5f5f5;
      margin: 0;
      padding: 0;
    }
    
    h3 {
      color: #333;
    }
    
    h2 {
      color: #007bff;
    }
    
    p#instructions {
      font-size: 18px;
      margin-top: 20px;
    }
    
    #initial-input {
      margin-top: 30px;
      padding: 20px;
      background-color: #fff;
      border-radius: 10px;
      box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.2);
      max-width: 400px;
      margin: 0 auto;
    }
    
    input[type="number"] {
      width: 100%;
      padding: 10px;
      margin: 5px 0;
      border: 1px solid #ccc;
      border-radius: 5px;
    }
    
    input[type="submit"] {
      background-color: #007bff;
      color: #fff;
      border: none;
      padding: 10px 20px;
      margin-top: 10px;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s;
    }
    
    input[type="submit"]:hover {
      background-color: #0056b3;
    }

    .disabled-button {
      background-color: #ccc;
      color: #888;
      padding: 3px 5px;
      margin-top: 10px;
      border-radius: 5px;
      cursor: not-allowed;
    }
    .enabled-button {
      background-color: #007bff;
      color: #fff;
      padding: 3px 5px;
      margin-top: 10px;
      border-radius: 5px;
      cursor: pointer;
    }
    .button-icon {
        font-size: 18px;
        vertical-align: middle;
        margin-right: 6px;
    }
  </style>
</head>

<body>
  <h1>&#x1F414;</h1>
  <h2>Chicken Coop Control</h2>
  <h3>Captive Portal</h1>
  <p id="instructions">
    Welcome to the CCC Portal.<br>
    Here you must input important information for the initialization of the Chicken Coop.
  <form id="initial-input" action="/get">
    <p id="inputs">
      &#9200; Date and Time:<br>
      <input type="datetime-local" id="datetime" name="datetime"><br><br>
      &#128206; Distance<br>(Between top-side of the Door and the CC-Controller):<br>
      <input type="number" name="distance"><br>
    </p>
    <p>Please click the button when you have finished:</p>
    <input type="submit" value="Submit">
  </form><br>
  <div id="button-container">
    <button action="/get" id="openButton" name="action" value="open" class="disabled-button">&#8593; Open</button>
    <button action="/get" id="closeButton" name="action" value="close" class="disabled-button">&#8595; Close</button>
  </div>

  <div id="dataContainer">
  Please finish to see data.
  </div><br>

<hr>
  <h3>&#128295; Advanced Options</h3>
  <form action="/get">
    <p>
      Sunset Hour (0-23):<br>
      <input type="number" name="sunsetHour" min="0" max="23">
    </p>
    <p>
      Sunset Minute (0-59):<br>
      <input type="number" name="sunsetMinute" min="0" max="59">
    </p>
    <p>
      Sunrise Hour (0-23):<br>
      <input type="number" name="sunriseHour" min="0" max="23">
    </p>
    <p>
      Sunrise Minute (0-59):<br>
      <input type="number" name="sunriseMinute" min="0" max="59">
    </p>
    <input type="submit" value="Update Options">
  </form>

</body>
</body></html>)rawliteral";

const char* login_html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Login</title>
</head>
<body>
  <h1>Login</h1>
  <form method="post" action="/login">
    <label for="username">Username:</label>
    <input type="text" id="username" name="username"><br><br>
    <label for="password">Password:</label>
    <input type="password" id="password" name="password"><br><br>
    <input type="submit" value="Login">
  </form>
</body>
</html>
)";

String pwInput = R"rawliteral(
    <form action="/get" method="GET">
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          background-color: #f5f5f5;
          margin: 0;
          padding: 0;
        }

        h3 {
          color: #333;
        }

        h2 {
          color: #007bff;
        }

        p#instructions {
          font-size: 18px;
          margin-top: 20px;
        }

        /* Add your custom CSS styles here for the password form */
        /* Example:
        input[type="password"] {
          width: 100%;
          padding: 10px;
          margin: 5px 0;
          border: 1px solid #ccc;
          border-radius: 5px;
        }
        input[type="submit"] {
          background-color: #007bff;
          color: #fff;
          border: none;
          padding: 10px 20px;
          margin-top: 10px;
          border-radius: 5px;
          cursor: pointer;
          transition: background-color 0.3s;
        }
        input[type="submit"]:hover {
          background-color: #0056b3;
        }
        */
      </style>
      &#x1F512; Password:
      <input type="password" name="password"><br>
      <input type="submit" value="Submit"><br>
        <div id="infoContainer"></div>
    </form>
  )rawliteral";
