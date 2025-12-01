[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/kA_iYo98)
# tJavaModule09fall25
tJavaModule09fall25

Mini Project: Spring Boot Weather Dashboard + “Vibe Programming” with AI
Overview

In this assignment, you will extend your existing Spring Boot project to build a tiny weather dashboard:

A simple HTML user interface (served by your Spring Boot app).

A button that asks your Spring Boot backend to look up the weather for a selected city.

The backend will call a public weather API (Open-Meteo) and return the current temperature. Open Meteo+1

You will use “vibe programming” with ChatGPT (or a similar AI coding assistant) to help you design, debug, and improve your code—but you must still understand and explain the code yourself.

Learning Goals

After completing this assignment, you should be able to:

Build a small full-stack feature with Spring Boot (Controller + HTML page).

Make an HTTP request from Java to a third-party REST API and read JSON data.

Connect a web UI control (a button) to a backend endpoint.

Use an AI assistant as a coding helper, not a shortcut, and explain how you used it.

What You Will Build

You will add a weather feature to your existing Spring Boot app:

A web page at something like:
http://localhost:8080/weather (or whatever port your app uses).

The page should include:

A title, e.g., “My Spring Boot Weather Dashboard”.

A drop-down list (select box) with at least three cities (e.g., Fresno, Los Angeles, New York).

A “Get Weather” button.

An area on the page where the current temperature and wind speed for that city will appear.

When the user picks a city and clicks the button:

The browser sends a request to your Spring Boot server (for example: GET /weather?city=Fresno).

Your Spring Boot code looks up the city’s coordinates (latitude and longitude) in your Java code.

Spring Boot calls the Open-Meteo Weather API (no API key needed) to get the current weather for those coordinates. Open Meteo+1

You extract the current temperature and wind speed and send that information back to the browser.

The browser displays something like:

Fresno – 20.3°C, wind 3.2 m/s

Part 0 – Starting Point

Use the Spring Boot project where you already:

Can run the app from IntelliJ.

Can open http://localhost:8080 (or another port) and see your previous assignment working.

If needed, you may:

Create a fresh Spring Boot project using Spring Initializr with at least:

Spring Web

Thymeleaf (optional but nice if you want server-side templates)

Part 1 – Represent the Cities in Your Java Code

Create a simple data structure in Java to hold a city’s:

Name (e.g., "Fresno")

Latitude

Longitude

You can do this in several ways (pick one we covered in class):

Option A (recommended): A small City class with three fields.

Option B: A Map<String, double[]> where the key is the city name and the array holds [lat, lon].

Hard-code at least three cities in your controller or in a separate helper class. For example:

Fresno – latitude 36.7378, longitude –119.7871

Los Angeles – latitude 34.0522, longitude –118.2437

New York – latitude 40.7128, longitude –74.0060

(You may look up more cities if you’d like.)

Part 2 – Call the Open-Meteo Weather API from Spring Boot

We will use Open-Meteo because it is free and does not require an API key. Open Meteo+1

The base URL (example) looks like this:

https://api.open-meteo.com/v1/forecast ?latitude=36.7378 &longitude=-119.7871 &current=temperature_2m,wind_speed_10m
Your task in Java

In your controller, create a method that:

Accepts a city name (for example, from a query parameter ?city=Fresno).

Looks up the city’s latitude and longitude.

Builds the URL to call Open-Meteo with current=temperature_2m,wind_speed_10m.

Use an HTTP client in Java to call the API:

You can use RestTemplate, WebClient, or any other simple library your instructor approves.

Your method should parse the JSON response and extract:

current.temperature_2m

current.wind_speed_10m

Return that data to your HTML page. You may:

Simple version (OK for this class):
Have your controller return a Thymeleaf view or simple HTML page. Put the temperature and wind speed into the model and show them when the page reloads.

Advanced version (optional):
Create a REST endpoint that returns JSON, then use JavaScript fetch() to update the page without reloading.

Part 3 – Build a Simple HTML User Interface

Create an HTML page that:

Is served by Spring Boot (e.g., src/main/resources/templates/weather.html or static/weather.html depending on how you set it up).

Contains:

A heading: “Weather Dashboard”.

A short description of what the page does.

A <select> (drop-down) listing your cities.

A “Get Weather” button.

A placeholder or section on the page where the result will appear.

When the button is clicked:

Form submit approach (simpler):

Use a regular HTML <form> that sends a GET request to /weather.

Your controller reads @RequestParam("city") and returns the updated page with the weather included.

JS fetch approach (harder, optional):

The button calls a JavaScript function.

The function uses fetch('/api/weather?city=Fresno').

The page updates only the result text.

Add at least a little styling (background color, padding, font changes) so the page looks like a real mini-app. You are welcome to let ChatGPT propose some CSS and then tweak it.

Part 4 – “Vibe Programming” with ChatGPT (or Another AI Assistant)

For this assignment, you are allowed and encouraged to use ChatGPT as a “vibe programming” partner:

Ask it for:

Help designing the HTML layout.

Suggestions for Spring Boot controller methods.

Explanations of error messages.

Ideas for improving your UI or code structure.

However:

You must not blindly copy and paste entire solutions without understanding them.

You must be able to explain every line of code you turn in, in your own words.

Reflection (Required)

Create a short reflection document (about 200–300 words) and submit it along with your code. In your reflection, answer:

How did you use ChatGPT (or another AI assistant) in this project?
Give at least two specific prompts you used and summarize the responses.

Show one small code snippet (4–8 lines) that the AI helped you with.
In your own words, explain what each line does.

What is one thing you understand better now about Spring Boot, APIs, or web development because of this assignment?

Deliverables

Submit the following to Canvas:

Code

Your Spring Boot project (zipped) or a link to your Git repository.

The controller class that calls the weather API.

The HTML page(s) for your weather dashboard.

Screenshot(s)

A screenshot of your browser showing:

The URL bar with your localhost URL.

The weather page with a selected city and temperature/wind data displayed.

Reflection Document

A short Word/Google Doc or PDF answering the reflection questions in Part 4.

Grading Checklist (Example)

Total points: 100

Backend functionality (30 pts)

Correct endpoint(s) created in Spring Boot.

Successfully calls the Open-Meteo API with latitude/longitude.

Parses and uses the current temperature and wind speed values.

Frontend / UI (25 pts)

Page loads via Spring Boot at /weather (or similar).

Drop-down and button work to trigger a weather request.

Weather results are displayed clearly.

Basic styling (not default browser gray).

Code quality (20 pts)

Reasonable organization and naming.

Comments explaining key methods.

No obviously unused or copy-pasted blocks you cannot explain.

AI “Vibe Programming” reflection (20 pts)

Concrete description of how AI was used.

Example prompt and snippet provided and explained.

Evidence that you understand the code.

Professionalism (5 pts)

Project runs without crashes when the instructor clones/unzips it.

All requested files and screenshots are included.
