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
const MAX_DATA_POINTS = 100;
const DISPLAY_DATA_POINTS = 20;

// Data Store
const dataStore = {
  temperature: [],
  humidity: [],
  waterLevel: [],
  pH: [],
  light: [],
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
  "pH",
  "light",
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
    case "pH":
      return "#ff9800";
    case "light":
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
          x: { type: "linear", ticks: { maxRotation: 0 } },
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
  document.getElementById("pH").innerText = data.phValue || "0.0";
  document.getElementById("light").innerText = data.lightDetect || "0.0";
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

/*
const chartCanvas = document.getElementById("chartCanvas");
//const chartContext = chartCanvas.getContext("2d");

// Cấu hình Chart.js cho biểu đồ (Đây là ví dụ với biểu đồ đường)
const chartConfig = {
  type: "line",
  data: {
    labels: [], // Các nhãn trên trục X
    datasets: [
      {
        label: "Dữ liệu",
        borderColor: "#ff5722",
        backgroundColor: "rgba(255, 87, 34, 0.1)",
        data: [], // Dữ liệu đường biểu diễn
        fill: true,
        lineTension: 0.2,
        borderWidth: 2,
      },
    ],
  },
  options: {
    responsive: true,
    scales: {
      x: {
        grid: {
          color: "rgba(255, 255, 255, 0.1)",
        },
      },
      y: {
        grid: {
          color: "rgba(255, 255, 255, 0.1)",
        },
        ticks: {
          beginAtZero: true,
          color: "#fff",
        },
      },
    },
    plugins: {
      legend: {
        labels: {
          color: "#fff",
        },
      },
    },
  },
};

// Khởi tạo biểu đồ
//let chart = new Chart(chartContext, chartConfig);

document.addEventListener("DOMContentLoaded", () => {
  const infoBoxes = document.querySelectorAll(".info-box");
  const chartPopup = document.getElementById("chart-popup");
  const popupChartCanvas = document.getElementById("chartCanvas");

  let popupChartInstance = null; // Biến lưu trữ đối tượng biểu đồ Chart.js
  let currentInfoType = null; // Biến theo dõi mục thông tin hiện tại của popup đồ Chart.js

  // Dữ liệu biểu đồ tương ứng cho từng mục
  const chartData = {
    temperature: {
      labels: [],
      datasets: [
        {
          label: "Nhiệt độ (°C)",
          data: [],
          borderColor: "#ff5722",
          backgroundColor: "rgba(255, 87, 34, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    humidity: {
      labels: [],
      datasets: [
        {
          label: "Độ ẩm (%)",
          data: [],
          borderColor: "#4caf50",
          backgroundColor: "rgba(76, 175, 80, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    waterLevel: {
      labels: [],
      datasets: [
        {
          label: "Mực nước (%)",
          data: [],
          borderColor: "#00a1e5",
          backgroundColor: "rgba(0, 161, 229, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    pH: {
      labels: [],
      datasets: [
        {
          label: "Độ pH",
          data: [],
          borderColor: "#ff9800",
          backgroundColor: "rgba(255, 152, 0, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    light: {
      labels: [],
      datasets: [
        {
          label: "Ánh sáng (%)",
          data: [],
          borderColor: "#00bcd4",
          backgroundColor: "rgba(0, 188, 212, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    pumpConsumption: {
      labels: [],
      datasets: [
        {
          label: "Máy bơm - Dòng điện (A)",
          data: [],
          borderColor: "#ff5722",
          backgroundColor: "rgba(255, 87, 34, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    ledConsumption: {
      labels: [],
      datasets: [
        {
          label: "Đèn - Dòng điện (A)",
          data: [],
          borderColor: "#4caf50",
          backgroundColor: "rgba(76, 175, 80, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    mistConsumption: {
      labels: [],
      datasets: [
        {
          label: "Máy phun sương - Dòng điện (A)",
          data: [],
          borderColor: "#00a1e5",
          backgroundColor: "rgba(0, 161, 229, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
  };

  // Hàm thêm dữ liệu mới vào biểu đồ
  function addData(chart, label, value) {
    const maxDataCount = 100; // Giới hạn tối đa 100 mẫu
    const visibleDataCount = 20; // Hiển thị tối đa 20 mẫu

    chart.data.labels.push(label);
    chart.data.datasets[0].data.push(value);

    if (chart.data.labels.length > maxDataCount) {
      chart.data.labels.shift();
      chart.data.datasets[0].data.shift();
    }

    chart.options.scales.x.min =
      chart.data.labels.length > visibleDataCount
        ? chart.data.labels.length - visibleDataCount
        : 0;

    chart.options.scales.x.max = chart.data.labels.length;

    chart.update();
  }

  // Hàm tạo biểu đồ Chart.js
  function createChart(ctx, data) {
    return new Chart(ctx, {
      type: "line",
      data: data,
      options: {
        responsive: true,
        scales: {
          x: {
            type: "linear",
            ticks: { maxRotation: 0 },
          },
          y: {
            beginAtZero: true,
          },
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

  // Khởi tạo cập nhật dữ liệu tự động
  setInterval(() => {
    Object.keys(chartData).forEach((key) => {
      const randomValue = (Math.random() * 50).toFixed(1); // Giả lập giá trị mới
      const currentTime = new Date().toLocaleTimeString(); // Sử dụng thời gian làm nhãn
      if (popupChartInstance && currentInfoType === key) {
        addData(popupChartInstance, currentTime, randomValue);
      }
    });
  }, 1000); // Mỗi 10 giây cập nhật một lần

  // Xử lý hiển thị popup và biểu đồ tương ứng
  infoBoxes.forEach((infoBox) => {
    const infoType = infoBox.classList[1]; // Lấy kiểu thông tin từ class (temperature, humidity,...)
    if (!chartData[infoType]) return;

    infoBox.addEventListener("mouseenter", (e) => {
      if (currentInfoType === infoType) return;

      currentInfoType = infoType;
      chartPopup.style.display = "block";

      // Tính toán vị trí popup
      const popupWidth = chartPopup.offsetWidth;
      const popupHeight = chartPopup.offsetHeight;
      const screenWidth = window.innerWidth;
      const screenHeight = window.innerHeight;

      let posX = e.pageX + 15;
      let posY = e.pageY + 15;

      if (posX + popupWidth > screenWidth) posX = e.pageX - popupWidth - 15;
      if (posY + popupHeight > screenHeight) posY = e.pageY - popupHeight - 15;

      chartPopup.style.left = `${posX}px`;
      chartPopup.style.top = `${posY}px`;

      // Hủy biểu đồ cũ (nếu có)
      if (popupChartInstance) popupChartInstance.destroy();

      // Tạo biểu đồ mới
      popupChartInstance = createChart(
        popupChartCanvas.getContext("2d"),
        chartData[infoType]
      );
    });

    infoBox.addEventListener("mouseleave", () => {
      chartPopup.style.display = "none";
      currentInfoType = null; // Reset trạng thái
    });
  });

  // Cập nhật vị trí popup theo chuột
  document.addEventListener("mousemove", (e) => {
    if (chartPopup.style.display === "block") {
      const popupWidth = chartPopup.offsetWidth;
      const popupHeight = chartPopup.offsetHeight;
      const screenWidth = window.innerWidth;
      const screenHeight = window.innerHeight;

      let posX = e.pageX + 15;
      let posY = e.pageY + 15;

      if (posX + popupWidth > screenWidth) posX = e.pageX - popupWidth - 15;
      if (posY + popupHeight > screenHeight) posY = e.pageY - popupHeight - 15;

      chartPopup.style.left = `${posX}px`;
      chartPopup.style.top = `${posY}px`;
    }
  });
});

document.addEventListener("DOMContentLoaded", () => {
  // Lấy tất cả các phần tử biểu đồ
  const charts = document.querySelectorAll(".statistic-card");
  let currentChartIndex = 0;

  // Hiển thị biểu đồ đầu tiên
  charts[currentChartIndex].style.display = "block";

  // Nút "Trái" (prev) chuyển đến biểu đồ trước đó
  document.getElementById("prevChart").addEventListener("click", () => {
    charts[currentChartIndex].style.display = "none"; // Ẩn biểu đồ hiện tại
    currentChartIndex =
      currentChartIndex === 0 ? charts.length - 1 : currentChartIndex - 1; // Chuyển về biểu đồ trước đó
    charts[currentChartIndex].style.display = "block"; // Hiển thị biểu đồ mới
  });

  // Nút "Phải" (next) chuyển đến biểu đồ tiếp theo
  document.getElementById("nextChart").addEventListener("click", () => {
    charts[currentChartIndex].style.display = "none"; // Ẩn biểu đồ hiện tại
    currentChartIndex =
      currentChartIndex === charts.length - 1 ? 0 : currentChartIndex + 1; // Chuyển về biểu đồ tiếp theo
    charts[currentChartIndex].style.display = "block"; // Hiển thị biểu đồ mới
  });

  // Biểu đồ Nhiệt độ
  const temperatureCtx = document
    .getElementById("temperatureChart")
    .getContext("2d");
  const temperatureChart = new Chart(temperatureCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Nhiệt độ (°C)",
          data: [],
          borderColor: "#ff5722",
          backgroundColor: "rgba(255, 87, 34, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Độ ẩm
  const humidityCtx = document.getElementById("humidityChart").getContext("2d");
  const humidityChart = new Chart(humidityCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Độ ẩm (%)",
          data: [],
          borderColor: "#4caf50",
          backgroundColor: "rgba(76, 175, 80, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Mực nước
  const waterLevelCtx = document
    .getElementById("waterLevelChart")
    .getContext("2d");
  const waterLevelChart = new Chart(waterLevelCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Mực nước (%)",
          data: [],
          borderColor: "#00a1e5",
          backgroundColor: "rgba(0, 161, 229, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Độ pH
  const phCtx = document.getElementById("pHChart").getContext("2d");
  const phChart = new Chart(phCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Độ pH",
          data: [],
          borderColor: "#ff9800",
          backgroundColor: "rgba(255, 152, 0, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Ánh sáng
  const lightCtx = document.getElementById("lightChart").getContext("2d");
  const lightChart = new Chart(lightCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Ánh sáng (%)",
          data: [],
          borderColor: "#00bcd4",
          backgroundColor: "rgba(0, 188, 212, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Máy bơm - Dòng điện
  const pumpPowerCtx = document.getElementById("pumpChart").getContext("2d");
  const pumpPowerChart = new Chart(pumpPowerCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Máy bơm - Dòng điện (A)",
          data: [],
          borderColor: "#ff5722",
          backgroundColor: "rgba(255, 87, 34, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Đèn - Dòng điện
  const lightPowerCtx = document.getElementById("ledChart").getContext("2d");
  const lightPowerChart = new Chart(lightPowerCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Đèn - Dòng điện (A)",
          data: [],
          borderColor: "#4caf50",
          backgroundColor: "rgba(76, 175, 80, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });

  // Biểu đồ Máy phun sương - Dòng điện
  const mistPowerCtx = document.getElementById("mistChart").getContext("2d");
  const mistPowerChart = new Chart(mistPowerCtx, {
    type: "line",
    data: {
      labels: [],
      datasets: [
        {
          label: "Máy phun sương - Dòng điện (A)",
          data: [],
          borderColor: "#00a1e5",
          backgroundColor: "rgba(0, 161, 229, 0.1)",
          fill: true,
          lineTension: 0.2,
          borderWidth: 2,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.1)" } },
        y: {
          grid: { color: "rgba(255, 255, 255, 0.1)" },
          ticks: { beginAtZero: true, color: "#fff" },
        },
      },
      plugins: {
        legend: { labels: { color: "#fff" } },
      },
    },
  });
});
*/
