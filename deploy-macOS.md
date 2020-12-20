Vagrant.configure("2") do |config|
  config.vm.box = "0xbahamoot/ledger-deploy-box"
  # data
  config.vm.synced_folder "./data", "/vagrant_data"
  # usb
  config.vm.provider "virtualbox" do |vb|
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--usbehci", "on"]
  end
  config.vm.provider "virtualbox" do |vb|
    vb.customize ["usbfilter", "add", "0",
        "--target", :id,
        "--vendorid","0x2c97",
        "--name", "Nano S",
        "--productid", "0x1011"]
        # "--name", "Nano S Bootloader",
        # "--productid", "0x0001"]
  end