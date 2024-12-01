function switchTab(tabId) {
  // Ẩn tất cả các tab nội dung
  const tabs = document.querySelectorAll(".tab_content");
  tabs.forEach((tab) => tab.classList.remove("active"));

  // Hiển thị tab được chọn
  document.getElementById(tabId).classList.add("active");

  // Xóa trạng thái active khỏi tất cả liên kết
  const links = document.querySelectorAll(".side_link");
  links.forEach((link) => link.classList.remove("active"));

  // Đặt trạng thái active cho liên kết được chọn
  const activeLink = document.getElementById(`${tabId}_tab`);
  if (activeLink) activeLink.classList.add("active");
}
document.addEventListener("DOMContentLoaded", function () {
  const counters = document.querySelectorAll(".info-box .value");
  counters.forEach((counter) => {
    let start = 0;
    const end = parseFloat(counter.textContent); // Giá trị đích
    const duration = 1000; // Thời gian (ms)
    const increment = (end - start) / (duration / 20); // Giá trị tăng mỗi lần

    const updateCounter = () => {
      start += increment;
      if (start >= end) {
        counter.textContent = end;
      } else {
        counter.textContent = Math.round(start * 100) / 100; // Làm tròn 2 chữ số
        requestAnimationFrame(updateCounter);
      }
    };

    updateCounter();
  });
});
/*
document.addEventListener("DOMContentLoaded", () => {
  const currentDateElement = document.getElementById("current-date");

  // Lấy ngày tháng năm hiện tại
  const now = new Date();
  const dateString = now.toLocaleDateString("vi-VN", {
    weekday: "long",
    year: "numeric",
    month: "long",
    day: "numeric",
  });

  currentDateElement.textContent = `📅 Hôm nay: ${dateString}`;
});*/
const currentDate = new Date();
const monthNames = [
  "Tháng 1",
  "Tháng 2",
  "Tháng 3",
  "Tháng 4",
  "Tháng 5",
  "Tháng 6",
  "Tháng 7",
  "Tháng 8",
  "Tháng 9",
  "Tháng 10",
  "Tháng 11",
  "Tháng 12",
];
const weekdayNames = [
  "Chủ Nhật",
  "Thứ Hai",
  "Thứ Ba",
  "Thứ Tư",
  "Thứ Năm",
  "Thứ Sáu",
  "Thứ Bảy",
];

document.querySelector(".calendar-header").innerText =
  monthNames[currentDate.getMonth()];
document.querySelector(".calendar-day").innerText = String(
  currentDate.getDate()
).padStart(2, "0");
document.querySelector(".calendar-weekday").innerText =
  weekdayNames[currentDate.getDay()];

const leftPanel = document.querySelector(".left");
let isPanelOpen = false;

document.addEventListener("mousemove", (e) => {
  const mouseX = e.clientX; // Vị trí chuột trên màn hình

  // Nếu chuột vào gần cạnh trái màn hình
  if (mouseX < 50 && !isPanelOpen) {
    leftPanel.style.transform = "translateX(0)"; // Mở panel
    isPanelOpen = true;
  } else if (mouseX > 50 && isPanelOpen && !leftPanel.contains(e.target)) {
    // Nếu chuột rời khỏi vùng của panel và không ở trong vùng .left
    leftPanel.style.transform = "translateX(-210px)"; // Thu lại panel
    isPanelOpen = false;
  }
});
