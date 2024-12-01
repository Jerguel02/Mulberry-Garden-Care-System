import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
import {
  getDatabase,
  ref,
  onValue,
} from "https://www.gstatic.com/firebasejs/9.4.0/firebase-database.js";

// Firebase Configuration
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

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const firebaseRef = ref(db, "sensors");

// Constants
const MAX_DATA_POINTS = 30;
const DISPLAY_DATA_POINTS = 20;

// Data Store
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

// Chart Data & Color Function
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

// Setup Chart Data and Canvas
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

// Initialize Charts
const charts = {}; // Keep track of chart instances
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

// Update UI
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

// Add Data to Chart
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

// Update Charts from Data Store
function updateChartsFromStore() {
  const now = new Date();
  const label = now.toLocaleTimeString();
  chartElements.forEach((type) => {
    const values = dataStore[type];
    const value = values && values.length > 0 ? values[values.length - 1] : 0; // Kiểm tra mảng không rỗng và lấy giá trị mới nhất
    addData(type, label, value);
  });
}

// Fetch Data from Firebase and update dataStore
function fetchDataFromFirebase() {
  onValue(firebaseRef, (snapshot) => {
    const data = snapshot.val();
    if (data) {
      // Cập nhật dataStore với dữ liệu mới từ Firebase
      Object.keys(dataStore).forEach((key) => {
        const value = data[key] || 0; // Giá trị mới từ Firebase (hoặc 0 nếu không có dữ liệu)
        dataStore[key].push(value);

        // Giới hạn số điểm dữ liệu trong dataStore
        if (dataStore[key].length > MAX_DATA_POINTS) {
          dataStore[key].shift(); // Loại bỏ dữ liệu cũ nhất nếu quá số lượng tối đa
        }
      });
      console.log("Data updated from Firebase:", dataStore);

      // Cập nhật UI với dữ liệu mới
      updateUI(data);

      // Cập nhật biểu đồ sau khi dữ liệu được thêm vào dataStore
    } else {
      console.error("No data found in Firebase");
    }
  });
  updateChartsFromStore();
}

// Fetch data every 10 seconds
setInterval(fetchDataFromFirebase, 10000);

// Start Updating Charts
//updateChartsFromStore();

// Chart Navigation Logic (Previous/Next)
document.addEventListener("DOMContentLoaded", () => {
  const chartsContainer = document.querySelectorAll(".statistic-card");
  let currentChartIndex = 0;

  // Hiển thị biểu đồ đầu tiên
  chartsContainer[currentChartIndex].style.display = "block";

  // Nút "Trái" (prev) chuyển đến biểu đồ trước đó
  document.getElementById("prevChart").addEventListener("click", () => {
    chartsContainer[currentChartIndex].style.display = "none"; // Ẩn biểu đồ hiện tại
    currentChartIndex =
      currentChartIndex === 0
        ? chartsContainer.length - 1
        : currentChartIndex - 1; // Chuyển về biểu đồ trước đó
    chartsContainer[currentChartIndex].style.display = "block"; // Hiển thị biểu đồ mới
  });

  // Nút "Phải" (next) chuyển đến biểu đồ tiếp theo
  document.getElementById("nextChart").addEventListener("click", () => {
    chartsContainer[currentChartIndex].style.display = "none"; // Ẩn biểu đồ hiện tại
    currentChartIndex =
      currentChartIndex === chartsContainer.length - 1
        ? 0
        : currentChartIndex + 1; // Chuyển về biểu đồ tiếp theo
    chartsContainer[currentChartIndex].style.display = "block"; // Hiển thị biểu đồ mới
  });
});
window.addEventListener("load", (event) => {
  // Toggles
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

  // Font Size Options
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

  // Themes Options
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
