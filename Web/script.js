function switchTab(tabId) {
  const tabs = document.querySelectorAll(".tab_content");
  tabs.forEach((tab) => tab.classList.remove("active"));

  document.getElementById(tabId).classList.add("active");

  const links = document.querySelectorAll(".side_link");
  links.forEach((link) => link.classList.remove("active"));

  const activeLink = document.getElementById(`${tabId}_tab`);
  if (activeLink) activeLink.classList.add("active");
}
document.addEventListener("DOMContentLoaded", function () {
  const counters = document.querySelectorAll(".info-box .value");
  counters.forEach((counter) => {
    let start = 0;
    const end = parseFloat(counter.textContent);
    const duration = 1000;
    const increment = (end - start) / (duration / 20);

    const updateCounter = () => {
      start += increment;
      if (start >= end) {
        counter.textContent = end;
      } else {
        counter.textContent = Math.round(start * 100) / 100;
        requestAnimationFrame(updateCounter);
      }
    };

    updateCounter();
  });
});

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
  const mouseX = e.clientX;

  if (mouseX < 50 && !isPanelOpen) {
    leftPanel.style.transform = "translateX(0)";
    isPanelOpen = true;
  } else if (mouseX > 50 && isPanelOpen && !leftPanel.contains(e.target)) {
    leftPanel.style.transform = "translateX(-210px)";
    isPanelOpen = false;
  }
});
