<template>
    <aside id="toasts" class="flex fixed flex-col right-0 bottom-0 z-30 gap-4 m-4 transition-all duration-500">
        <fwb-toast v-for="toast in toasts" closable :class="[{ 'exit-toast': toast.exitAnimation }]" :key="toast.id"
            :type="toast.type" @close="removeToast(toast.id)" ref="toastRef">
            {{ toast.message }}
        </fwb-toast>
    </aside>
</template>

<script>
import { FwbToast } from 'flowbite-vue';

export default {
    data() {
        return {
            toasts: [],
            toastOffset: 0,
            toastHeight: 0,
        };
    },
    methods: {
        async showToast(type, message, time) {
            const toastID = Math.floor(Math.random() * 100000);
            const toast = {
                id: toastID,
                type: type,
                message: message,
                exitAnimation: false,
            };


            this.toasts.push(toast);

            this.$nextTick(() => {
                try {
                    const toastElement = this.$refs.toastRef[0].$el;
                    this.toastHeight = toastElement.getBoundingClientRect().height || 100;
                    console.log(this.toastHeight);
                    this.toastOffset -= this.toastHeight + 16;
                }
                catch (error) {
                    console.log(error);
                }
                document.getElementById("toasts").animate([
                    // key frames
                    { transform: `translateY(${-this.toastOffset}px)` },
                    { transform: 'translateY(0px)' }
                ], {
                    // sync options
                    duration: 500,
                    iterations: 1,
                });

                setTimeout(() => {
                    if (this.toastOffset < 0) {
                        this.toastOffset += this.toastHeight + 16;
                    }
                }, 500);



                setTimeout(() => {
                    this.removeToast(toastID);
                }, time);

            });


        },
        removeToast(id) {
            const index = this.toasts.findIndex((toast) => toast.id === id);
            if (index !== -1) {
                this.toasts[index].exitAnimation = true;
                setTimeout(() => {
                    this.toasts = this.toasts.filter((toast) => toast.id !== id);
                }, 500); // Adjust timing based on the exit animation duration
            }
        },
        async displayToastsLoop() {
            while (true) {
                await new Promise((resolve) => setTimeout(resolve, 3000)); // 3-second delay
                await this.showToast('success', 'Success toast', 7000);
                await new Promise((resolve) => setTimeout(resolve, 3000)); // 3-second delay
                await this.showToast('warning', 'Warning toast', 7000);
                await new Promise((resolve) => setTimeout(resolve, 3000)); // 3-second delay
                await this.showToast('danger', 'Error toast', 7000);
            }
        },
    },
    mounted() {
        eel.expose(this.showToast, "show_toast");
    },
    components: {
        FwbToast
    }
};
</script>

<style>
.exit-toast {
    animation: slideRight .5s forwards;
}

@keyframes slideRight {
    from {
        transform: translateX(0);
        opacity: 1;
    }

    to {
        transform: translateX(100%);
        opacity: 0;
    }
}
</style>