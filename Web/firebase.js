import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
import {
  getDatabase,
  ref,
  onValue,
} from "https://www.gstatic.com/firebasejs/9.4.0/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyCcmsIiBMejWivNKMthf5kWbzLKeTKJ2sw",
  authDomain: "smartgarden-86bab.firebaseapp.com",
  databaseURL: "https://smartgarden-86bab-default-rtdb.firebaseio.com",
  projectId: "smartgarden-86bab",
  storageBucket: "smartgarden-86bab.appspot.com",
  messagingSenderId: "552607532450",
  appId: "1:552607532450:web:cbec61b814ac80409fdefb",
  measurementId: "G-CYKV77CV4W",
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const firebaseRef = ref(db, "sensors");

const MAX_DATA_POINTS = 30;
const DISPLAY_DATA_POINTS = 20;

const dataStore = {
  temperature: [],
  humidity: [],
  waterLevel: [],
  phValue: [],
  lightDetect: [],
  pumpConsumption: [],
  ledConsumption: [],
  mistConsumption: [],
};

const chartData = {};
const chartElements = [
  "temperature",
  "humidity",
  "waterLevel",
  "phValue",
  "lightDetect",
  "pumpConsumption",
  "ledConsumption",
  "mistConsumption",
];

function getColor(type) {
  switch (type) {
    case "temperature":
      return "#ff5722";
    case "humidity":
      return "#4caf50";
    case "waterLevel":
      return "#00a1e5";
    case "phValue":
      return "#ff9800";
    case "lightDetect":
      return "#00bcd4";
    case "pumpConsumption":
    case "ledConsumption":
    case "mistConsumption":
      return "#4caf50";
    default:
      return "#000";
  }
}

chartElements.forEach((type) => {
  chartData[type] = {
    labels: [],
    datasets: [
      {
        label: `${type.charAt(0).toUpperCase() + type.slice(1)} Data`,
        data: [],
        borderColor: getColor(type),
        backgroundColor: `${getColor(type)}33`,
        fill: true,
        lineTension: 0.2,
        borderWidth: 2,
      },
    ],
  };
});

const charts = {};
chartElements.forEach((type) => {
  const canvas = document.getElementById(`${type}Chart`);
  if (canvas) {
    const ctx = canvas.getContext("2d");
    charts[type] = new Chart(ctx, {
      type: "line",
      data: chartData[type],
      options: {
        responsive: true,
        scales: {
          x: { type: "category", ticks: { maxRotation: 0 } },
          y: { beginAtZero: true },
        },
        plugins: {
          legend: { labels: { color: "#fff" } },
          zoom: {
            pan: { enabled: true, mode: "x" },
            zoom: {
              wheel: { enabled: true },
              pinch: { enabled: true },
              mode: "x",
            },
          },
        },
      },
    });
  }
});

function updateUI(data) {
  document.getElementById("temperature").innerText = data.temperature || "0.0";
  document.getElementById("humidity").innerText = data.humidity || "0.0";
  document.getElementById("waterLevel").innerText = data.waterLevel || "0.0";
  document.getElementById("phValue").innerText = data.phValue || "0.0";
  document.getElementById("lightDetect").innerText = data.lightDetect
    ? "Yes"
    : "No";
  document.getElementById("pumpConsumption").innerText =
    data.pumpConsumption || "0.0";
  document.getElementById("ledConsumption").innerText =
    data.ledConsumption || "0.0";
  document.getElementById("mistConsumption").innerText =
    data.mistConsumption || "0.0";
}

function addData(type, label, value) {
  const chart = charts[type];
  chart.data.labels.push(label);
  chart.data.datasets[0].data.push(value);

  if (chart.data.labels.length > MAX_DATA_POINTS) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }
  if (chart.data.labels.length !== chart.data.datasets[0].data.length) {
    console.error(`Mismatched data: ${type}`);
  }

  console.log(`Adding data to ${type}:`, { label, value });
  console.log("Chart Update!");
  chart.update();
}

function updateChartsFromStore() {
  const now = new Date();
  const label = now.toLocaleTimeString();
  chartElements.forEach((type) => {
    const values = dataStore[type];
    const value = values && values.length > 0 ? values[values.length - 1] : 0;
    addData(type, label, value);
  });
}

function fetchDataFromFirebase() {
  onValue(firebaseRef, (snapshot) => {
    const data = snapshot.val();
    if (data) {
      Object.keys(dataStore).forEach((key) => {
        const value = data[key] || 0;
        dataStore[key].push(value);

        if (dataStore[key].length > MAX_DATA_POINTS) {
          dataStore[key].shift();
        }
      });
      console.log("Data updated from Firebase:", dataStore);

      updateUI(data);
    } else {
      console.error("No data found in Firebase");
    }
  });
  updateChartsFromStore();
}

setInterval(fetchDataFromFirebase, 10000);

document.addEventListener("DOMContentLoaded", () => {
  const chartsContainer = document.querySelectorAll(".statistic-card");
  let currentChartIndex = 0;

  chartsContainer[currentChartIndex].style.display = "block";

  document.getElementById("prevChart").addEventListener("click", () => {
    chartsContainer[currentChartIndex].style.display = "none";
    currentChartIndex =
      currentChartIndex === 0
        ? chartsContainer.length - 1
        : currentChartIndex - 1;
    chartsContainer[currentChartIndex].style.display = "block";
  });

  document.getElementById("nextChart").addEventListener("click", () => {
    chartsContainer[currentChartIndex].style.display = "none";
    currentChartIndex =
      currentChartIndex === chartsContainer.length - 1
        ? 0
        : currentChartIndex + 1;
    chartsContainer[currentChartIndex].style.display = "block";
  });
});
window.addEventListener("load", (event) => {
  var toggle = document.querySelectorAll(".toggle");

  toggle.forEach(function (el) {
    el.addEventListener("click", activateToggle);
  });

  function activateToggle(event) {
    var currentToggle = event.target;
    var toggleLabel = currentToggle.nextElementSibling;
    if (!toggleLabel) {
      console.error("Không tìm thấy nhãn liên quan!");
      return;
    }
    if (currentToggle.classList.contains("on")) {
      currentToggle.classList.remove("on");
      toggleLabel.style.color = "#b1b2d6";
      console.log("Removed on!");
    } else {
      currentToggle.classList.add("on");
      toggleLabel.style.color = "#4caf50";
      console.log("Added on!");
    }
  }

  var letter = document.querySelectorAll(".letter"),
    sizeS = document.querySelector(".size_s"),
    sizeM = document.querySelector(".size_m"),
    sizeL = document.querySelector(".size_l"),
    container = document.querySelector(".container");

  letter.forEach(function (el) {
    el.addEventListener("click", activateLetter);
  });

  function activateLetter(event) {
    var currentLetter = event.currentTarget,
      allLetters = document.querySelectorAll(".letter");

    allLetters.forEach(function (el) {
      el.classList.remove("select");
    });
    currentLetter.classList.add("select");

    if (sizeS.classList.contains("select")) {
      container.setAttribute("data-size", "small");
    }

    if (sizeM.classList.contains("select")) {
      container.setAttribute("data-size", "");
    }
    if (sizeL.classList.contains("select")) {
      container.setAttribute("data-size", "large");
    }
  }

  var color = document.querySelectorAll(".color"),
    colorPurple = document.querySelector(".c_purple"),
    colorGreen = document.querySelector(".c_green"),
    colorBlue = document.querySelector(".c_blue"),
    colorPink = document.querySelector(".c_pink"),
    colorOrange = document.querySelector(".c_orange");

  color.forEach(function (el) {
    el.addEventListener("click", changeTheme);
  });

  function changeTheme(event) {
    var currentColor = event.target,
      allColors = document.querySelectorAll(".color");

    allColors.forEach(function (el) {
      el.classList.remove("active_color");
    });
    currentColor.classList.add("active_color");

    if (colorPurple.classList.contains("active_color")) {
      container.setAttribute("data-theme", "");
    }

    if (colorGreen.classList.contains("active_color")) {
      container.setAttribute("data-theme", "green");
    }
    if (colorBlue.classList.contains("active_color")) {
      container.setAttribute("data-theme", "blue");
    }
    if (colorPink.classList.contains("active_color")) {
      container.setAttribute("data-theme", "pink");
    }
    if (colorOrange.classList.contains("active_color")) {
      container.setAttribute("data-theme", "orange");
    }
  }
});

async function fetchWeatherData() {
  const apiUrl =
    "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/HoChiMinh/tomorrow?unitGroup=metric&key=YA3D7X8ZFGNG7XTADSNQH7C43&contentType=json";

  try {
    const response = await fetch(apiUrl);
    const data = await response.json();
    return data.days[0].hours; // Trả về dữ liệu thời tiết theo giờ
  } catch (error) {
    console.error("Lỗi khi lấy dữ liệu thời tiết:", error);
    alert("Không thể lấy dữ liệu thời tiết!");
    return [];
  }
}

function populateTimeSelect(hours) {
  const timeSelect = document.getElementById("timeSelect");
  hours.forEach((hour, index) => {
    const option = document.createElement("option");
    option.value = index;
    option.textContent = `${hour.datetime}`;
    timeSelect.appendChild(option);
  });
}

function displayWeather(hourData) {
  const weatherDetails = document.getElementById("weatherDetails");
  if (!hourData) {
    weatherDetails.innerHTML = "<p>Không có dữ liệu thời tiết cho giờ này.</p>";
    return;
  }

  weatherDetails.innerHTML = `
    <p style = "font-size: 18px"><strong>Thời gian:</strong> ${hourData.datetime}</p>
    <p style = "font-size: 18px"><strong>Nhiệt độ:</strong> ${hourData.temp}°C</p>
    <p style = "font-size: 18px"><strong>Cảm giác như:</strong> ${hourData.feelslike}°C</p>
    <p style = "font-size: 18px"><strong>Độ ẩm:</strong> ${hourData.humidity}%</p>
    <p style = "font-size: 18px"><strong>Lượng mưa:</strong> ${hourData.precip} mm</p>
    <p style = "font-size: 18px"><strong>Xác suất mưa:</strong> ${hourData.precipprob}%</p>
    <p style = "font-size: 18px"><strong>Thời tiết:</strong> ${hourData.conditions}</p>
  `;
}

async function initializeWeatherForecast() {
  const hours = await fetchWeatherData();
  populateTimeSelect(hours);

  const timeSelect = document.getElementById("timeSelect");
  timeSelect.addEventListener("change", (event) => {
    const selectedHour = hours[event.target.value];
    displayWeather(selectedHour);
  });
}

initializeWeatherForecast();
